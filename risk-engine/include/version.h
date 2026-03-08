#ifndef VERSION_H
#define VERSION_H

#include <stdint.h>

#define RISK_SCHEMA_VERSION  1
#define RISK_ENGINE_VERSION  1

#define POLICY_HASH_SIZE 32

/*
Policy metadata binds the engine version to a cryptographic hash.
This prevents silent tampering or rollback.
*/
typedef struct {
    uint32_t schema_version;
    uint32_t engine_version;
    uint8_t integrity_hash[POLICY_HASH_SIZE];
} PolicyMetadata;

/* Initialize policy metadata */
void policy_metadata_init(PolicyMetadata *meta);

/* Verify policy integrity */
int policy_metadata_verify(const PolicyMetadata *meta);

#endif