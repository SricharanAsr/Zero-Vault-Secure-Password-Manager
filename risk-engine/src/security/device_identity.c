#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "security/device_identity.h"
#include "crypto/sha256.h"

static void random_bytes(uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
        buf[i] = rand() & 0xFF;
}

int device_identity_generate(DeviceIdentity *id)
{
    if (!id)
        return -1;

    srand((unsigned)time(NULL));

    random_bytes(id->device_key, DEVICE_KEY_SIZE);

    return 0;
}

int device_challenge_generate(DeviceChallenge *challenge)
{
    if (!challenge)
        return -1;

    random_bytes(challenge->nonce, DEVICE_NONCE_SIZE);

    return 0;
}

int device_response_create(
    const DeviceIdentity *id,
    const DeviceChallenge *challenge,
    DeviceResponse *response)
{
    if (!id || !challenge || !response)
        return -1;

    Sha256Ctx ctx;

    sha256_init(&ctx);

    sha256_update(&ctx, id->device_key, DEVICE_KEY_SIZE);
    sha256_update(&ctx, challenge->nonce, DEVICE_NONCE_SIZE);

    sha256_final(&ctx, response->tag);

    return 0;
}

int device_response_verify(
    const DeviceIdentity *id,
    const DeviceChallenge *challenge,
    const DeviceResponse *response)
{
    if (!id || !challenge || !response)
        return -1;

    DeviceResponse expected;

    device_response_create(id, challenge, &expected);

    if (memcmp(expected.tag, response->tag, DEVICE_TAG_SIZE) == 0)
        return 0;

    return -1;
}