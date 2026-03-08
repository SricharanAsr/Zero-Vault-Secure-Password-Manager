#ifndef DEVICE_IDENTITY_H
#define DEVICE_IDENTITY_H

#include <stdint.h>
#include <stddef.h>

#define DEVICE_KEY_SIZE 32
#define DEVICE_NONCE_SIZE 16
#define DEVICE_TAG_SIZE 32

typedef struct {
    uint8_t device_key[DEVICE_KEY_SIZE];
} DeviceIdentity;

typedef struct {
    uint8_t nonce[DEVICE_NONCE_SIZE];
} DeviceChallenge;

typedef struct {
    uint8_t tag[DEVICE_TAG_SIZE];
} DeviceResponse;

/* generate device identity */
int device_identity_generate(DeviceIdentity *id);

/* create authentication challenge */
int device_challenge_generate(DeviceChallenge *challenge);

/* compute challenge response */
int device_response_create(
    const DeviceIdentity *id,
    const DeviceChallenge *challenge,
    DeviceResponse *response
);

/* verify device response */
int device_response_verify(
    const DeviceIdentity *id,
    const DeviceChallenge *challenge,
    const DeviceResponse *response
);

#endif