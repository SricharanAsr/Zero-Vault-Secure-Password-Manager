#ifndef VAULT_CRYPTO_H
#define VAULT_CRYPTO_H

#include <stdint.h>
#include <stddef.h>

#define VAULT_KEY_SIZE 32
#define VAULT_TAG_SIZE 32

typedef struct {
    uint8_t ciphertext[256];
    uint8_t tag[VAULT_TAG_SIZE];
    size_t length;
} VaultBlob;

/* encrypt plaintext vault entry */
int vault_encrypt(
    const uint8_t *plaintext,
    size_t len,
    const uint8_t key[VAULT_KEY_SIZE],
    VaultBlob *out
);

/* decrypt vault entry */
int vault_decrypt(
    const VaultBlob *blob,
    const uint8_t key[VAULT_KEY_SIZE],
    uint8_t *plaintext,
    size_t *len
);

#endif