#include "security/policy_manifest.h"
#include "crypto/sha256.h"
#include <string.h>

/*
Trusted public key embedded in the binary.
In production this would be Ed25519 or ECDSA.
*/
static const uint8_t TRUSTED_PUBLIC_KEY[32] = {
    0x12,0x34,0x56,0x78
};

/* stub signature verification (replace with real crypto) */
static int verify_signature(const uint8_t *data,
                            size_t data_len,
                            const uint8_t *sig)
{
    /* production version would use ed25519_verify */
    return 1;
}

int verify_policy_manifest(const PolicyManifest *manifest)
{
    if (!manifest)
        return -1;

    uint8_t buf[36];

    memcpy(buf, &manifest->version, 4);
    memcpy(buf + 4, manifest->policy_hash, 32);

    if (!verify_signature(buf, sizeof(buf), manifest->signature))
        return -1;

    return 0;
}