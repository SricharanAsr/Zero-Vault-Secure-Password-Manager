#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "audit/record.h"
#include "crypto/sha256.h"

static int recompute_record_hash(const AuditRecord* rec,
                                 uint8_t out[AUDIT_HASH_SIZE])
{
    Sha256Ctx ctx;
    sha256_init(&ctx);

    sha256_update(&ctx,
        (const uint8_t*)&rec->engine_version,
        sizeof(rec->engine_version));

    sha256_update(&ctx,
        (const uint8_t*)&rec->record_index,
        sizeof(rec->record_index));

    sha256_update(&ctx,
        (const uint8_t*)&rec->decision,
        sizeof(rec->decision));

    sha256_update(&ctx,
        rec->risk_input_hash,
        AUDIT_HASH_SIZE);

    sha256_update(&ctx,
        rec->previous_record_hash,
        AUDIT_HASH_SIZE);

    sha256_final(&ctx, out);
    return 0;
}

int main(void)
{
    FILE* f = fopen("audit.log", "rb");
    if (!f) {
        perror("audit.log");
        return 1;
    }

    AuditRecord prev;
    AuditRecord curr;

    uint64_t expected_index = 1;
    int first = 1;

    while (fread(&curr, sizeof(curr), 1, f) == 1) {

        /* 1. Check monotonic index */
        if (curr.record_index != expected_index) {
            printf(" Index error at record %llu (expected %llu)\n",
                   (unsigned long long)curr.record_index,
                   (unsigned long long)expected_index);
            fclose(f);
            return 2;
        }

        /* 2. Verify previous hash chaining */
        if (!first) {
            if (memcmp(curr.previous_record_hash,
                       prev.record_hash,
                       AUDIT_HASH_SIZE) != 0) {
                printf(" Hash chain broken at record %llu\n",
                       (unsigned long long)curr.record_index);
                fclose(f);
                return 3;
            }
        } else {
            /* First record must have zero previous hash */
            uint8_t zero[AUDIT_HASH_SIZE] = {0};
            if (memcmp(curr.previous_record_hash, zero, AUDIT_HASH_SIZE) != 0) {
                printf(" Invalid genesis previous hash\n");
                fclose(f);
                return 4;
            }
            first = 0;
        }

        /* 3. Recompute record hash */
        uint8_t recomputed[AUDIT_HASH_SIZE];
        recompute_record_hash(&curr, recomputed);

        if (memcmp(recomputed, curr.record_hash, AUDIT_HASH_SIZE) != 0) {
            printf(" Record hash mismatch at record %llu\n",
                   (unsigned long long)curr.record_index);
            fclose(f);
            return 5;
        }

        prev = curr;
        expected_index++;
    }

    fclose(f);

    printf(" Audit log verified successfully (%llu records)\n",
           (unsigned long long)(expected_index - 1));

    return 0;
}
