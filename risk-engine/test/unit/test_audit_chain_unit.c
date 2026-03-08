#include "unity/unity.h"

#include "audit/record.h"
#include "decision.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* functions implemented in src/audit/log.c */
extern int audit_log_append(
    RiskDecision decision,
    const uint8_t risk_input_hash[AUDIT_HASH_SIZE]);

extern int audit_log_verify_chain(void);

void setUp(void)
{
    /* start with a fresh log */
    remove("audit.log");
}

void tearDown(void)
{
}

/* ------------------------------------------------ */
/* Test: Valid hash chain should verify correctly   */
/* ------------------------------------------------ */

void test_audit_chain_valid(void)
{
    uint8_t hash[AUDIT_HASH_SIZE] = {1};

    audit_log_append(RISK_ALLOW, hash);
    audit_log_append(RISK_STEP_UP, hash);
    audit_log_append(RISK_DENY, hash);

    TEST_ASSERT_EQUAL(0, audit_log_verify_chain());
}

/* ------------------------------------------------ */
/* Test: Tampering must break the hash chain        */
/* ------------------------------------------------ */

void test_audit_chain_detects_tamper(void)
{
    uint8_t hash[AUDIT_HASH_SIZE] = {1};

    audit_log_append(RISK_ALLOW, hash);
    audit_log_append(RISK_STEP_UP, hash);

    FILE *f = fopen("audit.log", "rb+");
    TEST_ASSERT_NOT_NULL(f);

    /* move to second record */
    fseek(f, sizeof(AuditRecord), SEEK_SET);

    AuditRecord rec;

    fread(&rec, sizeof(AuditRecord), 1, f);

    /* corrupt the stored record hash */
    rec.record_hash[0] ^= 0xFF;

    /* write the corrupted record back */
    fseek(f, sizeof(AuditRecord), SEEK_SET);
    fwrite(&rec, sizeof(AuditRecord), 1, f);

    fclose(f);

    TEST_ASSERT_NOT_EQUAL(0, audit_log_verify_chain());
}

/* ------------------------------------------------ */

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_audit_chain_valid);
    RUN_TEST(test_audit_chain_detects_tamper);

    return UNITY_END();
}