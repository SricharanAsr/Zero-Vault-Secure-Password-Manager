#ifndef METADATA_CRYPTO_H
#define METADATA_CRYPTO_H

#include <stdint.h>
#include "input/validate.h"

#define METADATA_TAG_SIZE 32

typedef struct {
    uint8_t ciphertext[sizeof(RiskInputHeader)];
    uint8_t tag[METADATA_TAG_SIZE];
} ProtectedMetadata;

int metadata_encrypt(
    const RiskInputHeader *header,
    ProtectedMetadata *out
);

int metadata_decrypt(
    const ProtectedMetadata *meta,
    RiskInputHeader *out
);

#endif