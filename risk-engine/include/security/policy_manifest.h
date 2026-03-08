#ifndef POLICY_MANIFEST_H
#define POLICY_MANIFEST_H

#include <stdint.h>

#define POLICY_HASH_SIZE 32
#define POLICY_SIG_SIZE 64

typedef struct {
    uint32_t version;
    uint8_t policy_hash[POLICY_HASH_SIZE];
    uint8_t signature[POLICY_SIG_SIZE];
} PolicyManifest;

int verify_policy_manifest(const PolicyManifest *manifest);

#endif