#include "unity/unity.h"
#include "security/metadata_crypto.h"

void setUp(void) {}
void tearDown(void) {}

void test_metadata_encrypt_decrypt(void)
{
    RiskInputHeader header = {
        .schema_version = 1,
        .engine_version = 1,
        .signal_bitmap_hash = 1234
    };

    ProtectedMetadata meta;
    RiskInputHeader result;

    TEST_ASSERT_EQUAL(0, metadata_encrypt(&header, &meta));
    TEST_ASSERT_EQUAL(0, metadata_decrypt(&meta, &result));

    TEST_ASSERT_EQUAL_MEMORY(&header, &result, sizeof(header));
}

void test_metadata_tampering_detected(void)
{
    RiskInputHeader header = {
        .schema_version = 1,
        .engine_version = 1,
        .signal_bitmap_hash = 1234
    };

    ProtectedMetadata meta;
    metadata_encrypt(&header, &meta);

    meta.ciphertext[0] ^= 0xAA;

    RiskInputHeader out;

    TEST_ASSERT_NOT_EQUAL(0, metadata_decrypt(&meta, &out));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_metadata_encrypt_decrypt);
    RUN_TEST(test_metadata_tampering_detected);
    return UNITY_END();
}