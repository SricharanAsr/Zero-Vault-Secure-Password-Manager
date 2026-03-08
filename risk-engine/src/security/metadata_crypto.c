#include <string.h>

#include "security/metadata_crypto.h"
#include "crypto/sha256.h"
#include "version.h"

static const uint8_t META_KEY[32] = {
    0x12,0x43,0x76,0x21
};

/* simple XOR encryption */
static void xor_cipher(uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
        data[i] ^= META_KEY[i % sizeof(META_KEY)];
}

int metadata_encrypt(
    const RiskInputHeader *header,
    ProtectedMetadata *out
)
{
    if (!header || !out)
        return -1;

    memcpy(out->ciphertext, header, sizeof(RiskInputHeader));

    xor_cipher(out->ciphertext, sizeof(RiskInputHeader));

    Sha256Ctx ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, META_KEY, sizeof(META_KEY));
    sha256_update(&ctx, out->ciphertext, sizeof(RiskInputHeader));

    uint32_t version = RISK_ENGINE_VERSION;
    sha256_update(&ctx, (uint8_t*)&version, sizeof(version));

    sha256_final(&ctx, out->tag);

    return 0;
}

int metadata_decrypt(
    const ProtectedMetadata *meta,
    RiskInputHeader *out
)
{
    if (!meta || !out)
        return -1;

    uint8_t expected[METADATA_TAG_SIZE];

    Sha256Ctx ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, META_KEY, sizeof(META_KEY));
    sha256_update(&ctx, meta->ciphertext, sizeof(RiskInputHeader));

    uint32_t version = RISK_ENGINE_VERSION;
    sha256_update(&ctx, (uint8_t*)&version, sizeof(version));

    sha256_final(&ctx, expected);

    if (memcmp(expected, meta->tag, METADATA_TAG_SIZE) != 0)
        return -1;

    memcpy(out, meta->ciphertext, sizeof(RiskInputHeader));

    xor_cipher((uint8_t*)out, sizeof(RiskInputHeader));

    return 0;
}