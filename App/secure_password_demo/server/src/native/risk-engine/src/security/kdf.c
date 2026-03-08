#include <stdlib.h>
#include <string.h>

#include "security/kdf.h"
#include "crypto/sha256.h"

#define DOMAIN_ROOT  0x01
#define DOMAIN_VAULT 0x02
#define DOMAIN_ENTRY 0x03

static void hash_mix(
    const uint8_t *a,
    size_t alen,
    const uint8_t *b,
    size_t blen,
    uint8_t out[KDF_KEY_SIZE]
)
{
    Sha256Ctx ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, a, alen);
    sha256_update(&ctx, b, blen);
    sha256_final(&ctx, out);
}

int kdf_derive_root(
    const uint8_t *password,
    size_t pass_len,
    const uint8_t salt[KDF_SALT_SIZE],
    const KdfParams *params,
    uint8_t out[KDF_KEY_SIZE]
)
{
    if (!password || !salt || !params || !out)
        return -1;

    uint8_t state[KDF_KEY_SIZE];

    hash_mix(password, pass_len, salt, KDF_SALT_SIZE, state);

    uint8_t *memory = malloc(params->memory_blocks * KDF_KEY_SIZE);

    if (!memory)
        return -1;

    /* initialize memory */
    for (uint32_t i = 0; i < params->memory_blocks; i++)
    {
        hash_mix(state, KDF_KEY_SIZE,
                 (uint8_t*)&i, sizeof(i),
                 memory + (i * KDF_KEY_SIZE));
    }

    /* sequential memory mixing */
    for (uint32_t i = 0; i < params->iterations; i++)
    {
        uint32_t idx = i % params->memory_blocks;

        hash_mix(
            state, KDF_KEY_SIZE,
            memory + (idx * KDF_KEY_SIZE),
            KDF_KEY_SIZE,
            state
        );

        memcpy(
            memory + (idx * KDF_KEY_SIZE),
            state,
            KDF_KEY_SIZE
        );
    }

    /* domain separation */
    uint8_t domain = DOMAIN_ROOT;

    hash_mix(state, KDF_KEY_SIZE, &domain, 1, out);

    memset(memory, 0, params->memory_blocks * KDF_KEY_SIZE);
    free(memory);

    return 0;
}

int kdf_derive_vault_key(
    const uint8_t root[KDF_KEY_SIZE],
    uint8_t out[KDF_KEY_SIZE]
)
{
    uint8_t domain = DOMAIN_VAULT;

    hash_mix(root, KDF_KEY_SIZE, &domain, 1, out);

    return 0;
}

int kdf_derive_entry_key(
    const uint8_t vault_key[KDF_KEY_SIZE],
    uint32_t entry_id,
    uint8_t out[KDF_KEY_SIZE]
)
{
    uint8_t domain = DOMAIN_ENTRY;

    Sha256Ctx ctx;

    sha256_init(&ctx);

    sha256_update(&ctx, vault_key, KDF_KEY_SIZE);
    sha256_update(&ctx, &domain, 1);
    sha256_update(&ctx, (uint8_t*)&entry_id, sizeof(entry_id));

    sha256_final(&ctx, out);

    return 0;
}