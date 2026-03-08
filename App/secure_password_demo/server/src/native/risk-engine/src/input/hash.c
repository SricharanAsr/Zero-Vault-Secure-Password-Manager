#include <string.h>
#include <stdint.h>

#include "input/hash.h"
#include "crypto/sha256.h"

int compute_risk_input_hash(
    const RiskInput* input,
    uint8_t out[RISK_INPUT_HASH_SIZE]
) {
    Sha256Ctx ctx;
    sha256_init(&ctx);

    sha256_update(&ctx,
        (const uint8_t*)&input->header->schema_version,
        sizeof(uint16_t));
    sha256_update(&ctx,
        (const uint8_t*)&input->header->engine_version,
        sizeof(uint16_t));
    sha256_update(&ctx,
        (const uint8_t*)&input->header->signal_bitmap_hash,
        sizeof(uint32_t));

    for (size_t i = 0; i < input->signal_count; i++) {
        sha256_update(&ctx,
            (const uint8_t*)&input->signals[i].signal_id,
            sizeof(uint16_t));
        sha256_update(&ctx,
            (const uint8_t*)&input->signals[i].normalized_value,
            sizeof(int64_t));
    }

    sha256_final(&ctx, out);
    return 0;
}
