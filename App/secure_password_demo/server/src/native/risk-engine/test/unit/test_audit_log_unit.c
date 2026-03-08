#include "unity/unity.h"
#include "audit/record.h"
#include <stdio.h>

extern int audit_log_append(
    RiskDecision decision,
    const uint8_t risk_input_hash[AUDIT_HASH_SIZE]);

void setUp(void)
{
    remove("audit.log");
}

void tearDown(void) {}

void test_audit_log_append_creates_file(void)
{
    uint8_t hash[AUDIT_HASH_SIZE] = {1};

    TEST_ASSERT_EQUAL(
        0,
        audit_log_append(RISK_ALLOW, hash)
    );

    FILE *f = fopen("audit.log","rb");

    TEST_ASSERT_NOT_NULL(f);

    fclose(f);
}

void test_audit_log_append_increases_size(void)
{
    uint8_t hash[AUDIT_HASH_SIZE] = {1};

    audit_log_append(RISK_ALLOW, hash);

    FILE *f = fopen("audit.log","rb");
    fseek(f,0,SEEK_END);
    long size1 = ftell(f);
    fclose(f);

    audit_log_append(RISK_ALLOW, hash);

    f = fopen("audit.log","rb");
    fseek(f,0,SEEK_END);
    long size2 = ftell(f);
    fclose(f);

    TEST_ASSERT_TRUE(size2 > size1);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_audit_log_append_creates_file);
    RUN_TEST(test_audit_log_append_increases_size);

    return UNITY_END();
}