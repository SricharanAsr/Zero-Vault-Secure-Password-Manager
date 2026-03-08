#ifndef CRYPTO_SHA256_H
#define CRYPTO_SHA256_H

#include <stdint.h>
#include <stddef.h>

#define SHA256_HASH_SIZE 32

typedef struct {
    uint32_t state[8];
    uint64_t bitlen;
    uint8_t  data[64];
    size_t   datalen;
} Sha256Ctx;

void sha256_init(Sha256Ctx* ctx);
void sha256_update(Sha256Ctx* ctx, const uint8_t* data, size_t len);
void sha256_final(Sha256Ctx* ctx, uint8_t hash[SHA256_HASH_SIZE]);

#endif
