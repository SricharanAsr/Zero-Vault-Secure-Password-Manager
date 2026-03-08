#ifndef AUDIT_RECORD_H
#define AUDIT_RECORD_H

#include <stdint.h>
#include "decision.h"

#define AUDIT_HASH_SIZE 32

#pragma pack(push, 1)
typedef struct {

    uint16_t engine_version;

    uint64_t record_index;

    uint16_t decision;

    uint8_t risk_input_hash[AUDIT_HASH_SIZE];

    uint8_t previous_record_hash[AUDIT_HASH_SIZE];

    uint8_t record_hash[AUDIT_HASH_SIZE];

} AuditRecord;
#pragma pack(pop)

/* -------- User Story 1.8 -------- */
/* Verify the entire audit log hash chain */
int audit_log_verify_chain(void);

#endif