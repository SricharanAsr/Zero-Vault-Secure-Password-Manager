#include "unity/unity.h"
#include "security/vault_crypto.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_vault_encrypt_decrypt(void)
{
    uint8_t key[VAULT_KEY_SIZE] = {1,2,3,4};

    const char *msg = "secret_password";

    VaultBlob blob;

    TEST_ASSERT_EQUAL(
        0,
        vault_encrypt(
            (const uint8_t*)msg,
            strlen(msg),
            key,
            &blob
        )
    );

    uint8_t out[256];
    size_t len;

    TEST_ASSERT_EQUAL(
        0,
        vault_decrypt(&blob, key, out, &len)
    );

    TEST_ASSERT_EQUAL_MEMORY(msg, out, len);
}

void test_vault_tamper_detected(void)
{
    uint8_t key[VAULT_KEY_SIZE] = {1,2,3};

    const char *msg = "secret";

    VaultBlob blob;

    vault_encrypt((const uint8_t*)msg, strlen(msg), key, &blob);

    blob.ciphertext[0] ^= 0xAA;

    uint8_t out[256];
    size_t len;

    TEST_ASSERT_NOT_EQUAL(
        0,
        vault_decrypt(&blob, key, out, &len)
    );
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_vault_encrypt_decrypt);
    RUN_TEST(test_vault_tamper_detected);

    return UNITY_END();
}