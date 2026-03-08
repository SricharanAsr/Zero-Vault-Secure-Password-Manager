#include <string.h>

#include "security/vault_crypto.h"
#include "crypto/sha256.h"

/* simple XOR cipher for demonstration */
static void xor_cipher(uint8_t *data, size_t len, const uint8_t *key)
{
    for (size_t i = 0; i < len; i++)
        data[i] ^= key[i % VAULT_KEY_SIZE];
}

int vault_encrypt(
    const uint8_t *plaintext,
    size_t len,
    const uint8_t key[VAULT_KEY_SIZE],
    VaultBlob *out
)
{
    if (!plaintext || !key || !out)
        return -1;

    if (len > sizeof(out->ciphertext))
        return -1;

    memcpy(out->ciphertext, plaintext, len);
    out->length = len;

    xor_cipher(out->ciphertext, len, key);

    Sha256Ctx ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, key, VAULT_KEY_SIZE);
    sha256_update(&ctx, out->ciphertext, len);
    sha256_final(&ctx, out->tag);

    return 0;
}

int vault_decrypt(
    const VaultBlob *blob,
    const uint8_t key[VAULT_KEY_SIZE],
    uint8_t *plaintext,
    size_t *len
)
{
    if (!blob || !key || !plaintext || !len)
        return -1;

    uint8_t expected[VAULT_TAG_SIZE];

    Sha256Ctx ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, key, VAULT_KEY_SIZE);
    sha256_update(&ctx, blob->ciphertext, blob->length);
    sha256_final(&ctx, expected);

    if (memcmp(expected, blob->tag, VAULT_TAG_SIZE) != 0)
        return -1;

    memcpy(plaintext, blob->ciphertext, blob->length);

    xor_cipher(plaintext, blob->length, key);

    *len = blob->length;

    return 0;
}