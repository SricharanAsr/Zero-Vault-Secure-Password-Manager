#include <string.h>

#include "version.h"
#include "crypto/sha256.h"

/*
Compute a deterministic integrity hash based on policy version fields.
This binds the enforcement configuration to a cryptographic checksum.
*/
static void compute_policy_hash(
    uint32_t schema_version,
    uint32_t engine_version,
    uint8_t out[POLICY_HASH_SIZE]
)
{
    Sha256Ctx ctx;

    sha256_init(&ctx);

    sha256_update(&ctx, (uint8_t*)&schema_version, sizeof(schema_version));
    sha256_update(&ctx, (uint8_t*)&engine_version, sizeof(engine_version));

    sha256_final(&ctx, out);
}

void policy_metadata_init(PolicyMetadata *meta)
{
    if (!meta)
        return;

    meta->schema_version = RISK_SCHEMA_VERSION;
    meta->engine_version = RISK_ENGINE_VERSION;

    compute_policy_hash(
        meta->schema_version,
        meta->engine_version,
        meta->integrity_hash
    );
}

int policy_metadata_verify(const PolicyMetadata *meta)
{
    if (!meta)
        return -1;

    uint8_t expected[POLICY_HASH_SIZE];

    compute_policy_hash(
        meta->schema_version,
        meta->engine_version,
        expected
    );

    if (memcmp(expected, meta->integrity_hash, POLICY_HASH_SIZE) != 0)
        return -1;

    if (meta->schema_version != RISK_SCHEMA_VERSION)
        return -1;

    if (meta->engine_version != RISK_ENGINE_VERSION)
        return -1;

    return 0;
}