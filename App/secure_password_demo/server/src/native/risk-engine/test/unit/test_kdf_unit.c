#include "unity/unity.h"
#include "security/kdf.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_root_kdf_deterministic(void)
{
    uint8_t salt[KDF_SALT_SIZE] = {1,2,3};

    KdfParams params = {
        .iterations = 1000,
        .memory_blocks = 64
    };

    const char *pw = "master_password";

    uint8_t k1[KDF_KEY_SIZE];
    uint8_t k2[KDF_KEY_SIZE];

    kdf_derive_root((uint8_t*)pw, strlen(pw), salt, &params, k1);
    kdf_derive_root((uint8_t*)pw, strlen(pw), salt, &params, k2);

    TEST_ASSERT_EQUAL_MEMORY(k1, k2, KDF_KEY_SIZE);
}

void test_entry_keys_unique(void)
{
    uint8_t vault[KDF_KEY_SIZE] = {5};

    uint8_t k1[KDF_KEY_SIZE];
    uint8_t k2[KDF_KEY_SIZE];

    kdf_derive_entry_key(vault, 1, k1);
    kdf_derive_entry_key(vault, 2, k2);

    TEST_ASSERT_NOT_EQUAL(0, memcmp(k1, k2, KDF_KEY_SIZE));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_root_kdf_deterministic);
    RUN_TEST(test_entry_keys_unique);

    return UNITY_END();
}