#include <stdio.h>
#include <string.h>

#include "security/kdf.h"
#include "security/vault_crypto.h"

int main()
{
    printf("=========================================\n");
    printf(" Vault Encryption + KDF Integration Test\n");
    printf(" User Stories: 2.1 + 2.2\n");
    printf("=========================================\n\n");

    const char *master_password = "correct horse battery staple";

    uint8_t salt[KDF_SALT_SIZE] = {1,2,3,4};

    KdfParams params = {
        .iterations = 1000,
        .memory_blocks = 64
    };

    uint8_t root_key[KDF_KEY_SIZE];
    uint8_t vault_key[KDF_KEY_SIZE];
    uint8_t entry_key[KDF_KEY_SIZE];

    printf("Step 1: Derive root key from master password\n");

    if (kdf_derive_root(
            (const uint8_t*)master_password,
            strlen(master_password),
            salt,
            &params,
            root_key) != 0)
    {
        printf("FAIL: Root key derivation failed\n");
        return 1;
    }

    printf("PASS: Root key derived\n\n");

    printf("Step 2: Derive vault master key\n");

    kdf_derive_vault_key(root_key, vault_key);

    printf("PASS: Vault key derived\n\n");

    printf("Step 3: Derive entry key\n");

    kdf_derive_entry_key(vault_key, 1, entry_key);

    printf("PASS: Entry key derived\n\n");

    const char *secret = "vault_password_123";

    VaultBlob blob;

    printf("Step 4: Encrypt vault entry\n");

    if (vault_encrypt(
            (const uint8_t*)secret,
            strlen(secret),
            entry_key,
            &blob) != 0)
    {
        printf("FAIL: Encryption failed\n");
        return 1;
    }

    printf("PASS: Entry encrypted\n\n");

    printf("Step 5: Decrypt vault entry\n");

    uint8_t decrypted[256];
    size_t len;

    if (vault_decrypt(&blob, entry_key, decrypted, &len) != 0)
    {
        printf("FAIL: Decryption failed\n");
        return 1;
    }

    decrypted[len] = '\0';

    printf("Decrypted value: %s\n", decrypted);

    if (strcmp((char*)decrypted, secret) == 0)
        printf("PASS: Decrypted data matches original\n\n");
    else
        printf("FAIL: Data mismatch\n\n");

    printf("Step 6: Simulate tampering attack\n");

    blob.ciphertext[0] ^= 0xAA;

    if (vault_decrypt(&blob, entry_key, decrypted, &len) != 0)
        printf("PASS: Tampering detected (fail-secure)\n");
    else
        printf("FAIL: Tampering NOT detected\n");

    printf("\n=========================================\n");
    printf(" Integration Test Completed\n");
    printf("=========================================\n");

    return 0;
}