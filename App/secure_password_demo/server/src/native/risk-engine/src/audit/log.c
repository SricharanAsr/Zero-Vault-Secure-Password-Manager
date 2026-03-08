#include <stdio.h>
#include <string.h>

#include "audit/record.h"
#include "crypto/sha256.h"
#include "version.h"

#define AUDIT_LOG_FILE "audit.log"

static int read_last_record(FILE* f, AuditRecord* out)
{
    if (fseek(f, -(long)sizeof(AuditRecord), SEEK_END) != 0) {
        return -1;
    }
    if (fread(out, sizeof(AuditRecord), 1, f) != 1) {
        return -1;
    }
    return 0;
}

static void compute_record_hash(AuditRecord* rec)
{
    Sha256Ctx ctx;

    sha256_init(&ctx);

    sha256_update(&ctx, (uint8_t*)&rec->engine_version, sizeof(rec->engine_version));
    sha256_update(&ctx, (uint8_t*)&rec->record_index, sizeof(rec->record_index));
    sha256_update(&ctx, (uint8_t*)&rec->decision, sizeof(rec->decision));
    sha256_update(&ctx, rec->risk_input_hash, AUDIT_HASH_SIZE);
    sha256_update(&ctx, rec->previous_record_hash, AUDIT_HASH_SIZE);

    sha256_final(&ctx, rec->record_hash);
}

int audit_log_append(
    RiskDecision decision,
    const uint8_t risk_input_hash[AUDIT_HASH_SIZE])
{
    FILE* f = fopen(AUDIT_LOG_FILE, "ab+");
    if (!f) return -1;

    AuditRecord rec;
    memset(&rec, 0, sizeof(rec));

    rec.engine_version = RISK_ENGINE_VERSION;
    rec.decision = (uint16_t)decision;

    memcpy(rec.risk_input_hash, risk_input_hash, AUDIT_HASH_SIZE);

    AuditRecord prev;

    if (read_last_record(f, &prev) == 0) {
        rec.record_index = prev.record_index + 1;
        memcpy(rec.previous_record_hash, prev.record_hash, AUDIT_HASH_SIZE);
    }
    else {
        rec.record_index = 1;
        memset(rec.previous_record_hash, 0, AUDIT_HASH_SIZE);
    }

    compute_record_hash(&rec);

    if (fwrite(&rec, sizeof(rec), 1, f) != 1) {
        fclose(f);
        return -1;
    }

    fflush(f);
    fclose(f);

    return 0;
}

/* ========================================================= */
/* User Story 1.8: Verify Tamper-Evident Hash Chain           */
/* ========================================================= */

int audit_log_verify_chain(void)
{
    FILE* f = fopen(AUDIT_LOG_FILE, "rb");
    if (!f) {
        return -1;
    }

    AuditRecord prev;
    AuditRecord curr;

    if (fread(&prev, sizeof(AuditRecord), 1, f) != 1) {
        fclose(f);
        return -1;
    }

    while (fread(&curr, sizeof(AuditRecord), 1, f) == 1) {

        /* verify previous hash link */
        if (memcmp(curr.previous_record_hash,
                   prev.record_hash,
                   AUDIT_HASH_SIZE) != 0) {
            fclose(f);
            return -1;
        }

        /* verify current record hash */
        AuditRecord tmp = curr;
        compute_record_hash(&tmp);

        if (memcmp(tmp.record_hash,
                   curr.record_hash,
                   AUDIT_HASH_SIZE) != 0) {
            fclose(f);
            return -1;
        }

        prev = curr;
    }

    fclose(f);

    return 0;
}