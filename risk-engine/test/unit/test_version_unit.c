#include "unity/unity.h"
#include "version.h"
#include <string.h>

/*
Unit Tests for User Story 1.9
Version & Integrity Protection
*/

void setUp(void) {}
void tearDown(void) {}

/* Valid metadata should pass verification */
void test_valid_policy_metadata_passes(void)
{
    PolicyMetadata meta;

    policy_metadata_init(&meta);

    TEST_ASSERT_EQUAL(0, policy_metadata_verify(&meta));
}

/* Tampering with version should fail */
void test_version_tampering_detected(void)
{
    PolicyMetadata meta;

    policy_metadata_init(&meta);

    meta.engine_version = 999;

    TEST_ASSERT_NOT_EQUAL(0, policy_metadata_verify(&meta));
}

/* Tampering with integrity hash should fail */
void test_integrity_hash_tampering_detected(void)
{
    PolicyMetadata meta;

    policy_metadata_init(&meta);

    meta.integrity_hash[0] ^= 0xFF;

    TEST_ASSERT_NOT_EQUAL(0, policy_metadata_verify(&meta));
}

/* NULL input should fail safely */
void test_null_metadata_rejected(void)
{
    TEST_ASSERT_NOT_EQUAL(0, policy_metadata_verify(NULL));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_valid_policy_metadata_passes);
    RUN_TEST(test_version_tampering_detected);
    RUN_TEST(test_integrity_hash_tampering_detected);
    RUN_TEST(test_null_metadata_rejected);

    return UNITY_END();
}