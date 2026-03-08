#ifndef KDF_H
#define KDF_H

#include <stdint.h>
#include <stddef.h>

#define KDF_KEY_SIZE 32
#define KDF_SALT_SIZE 16

typedef struct {
    uint32_t iterations;
    uint32_t memory_blocks;
} KdfParams;

/* derive root key from master password */
int kdf_derive_root(
    const uint8_t *password,
    size_t pass_len,
    const uint8_t salt[KDF_SALT_SIZE],
    const KdfParams *params,
    uint8_t out[KDF_KEY_SIZE]
);

/* derive vault master key */
int kdf_derive_vault_key(
    const uint8_t root[KDF_KEY_SIZE],
    uint8_t out[KDF_KEY_SIZE]
);

/* derive entry key */
int kdf_derive_entry_key(
    const uint8_t vault_key[KDF_KEY_SIZE],
    uint32_t entry_id,
    uint8_t out[KDF_KEY_SIZE]
);

#endif