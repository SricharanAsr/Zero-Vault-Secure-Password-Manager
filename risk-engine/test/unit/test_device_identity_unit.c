#include "unity/unity.h"
#include "security/device_identity.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_device_identity_generation(void)
{
    DeviceIdentity id;

    TEST_ASSERT_EQUAL(0, device_identity_generate(&id));

    int all_zero = 1;

    for (int i = 0; i < DEVICE_KEY_SIZE; i++)
    {
        if (id.device_key[i] != 0)
        {
            all_zero = 0;
            break;
        }
    }

    TEST_ASSERT_FALSE(all_zero);
}

void test_device_challenge_generation(void)
{
    DeviceChallenge challenge;

    TEST_ASSERT_EQUAL(0, device_challenge_generate(&challenge));

    int all_zero = 1;

    for (int i = 0; i < DEVICE_NONCE_SIZE; i++)
    {
        if (challenge.nonce[i] != 0)
        {
            all_zero = 0;
            break;
        }
    }

    TEST_ASSERT_FALSE(all_zero);
}

void test_device_response_verification_success(void)
{
    DeviceIdentity id;
    DeviceChallenge challenge;
    DeviceResponse response;

    device_identity_generate(&id);
    device_challenge_generate(&challenge);

    TEST_ASSERT_EQUAL(
        0,
        device_response_create(&id, &challenge, &response)
    );

    TEST_ASSERT_EQUAL(
        0,
        device_response_verify(&id, &challenge, &response)
    );
}

void test_device_response_verification_fail_on_tamper(void)
{
    DeviceIdentity id;
    DeviceChallenge challenge;
    DeviceResponse response;

    device_identity_generate(&id);
    device_challenge_generate(&challenge);

    device_response_create(&id, &challenge, &response);

    /* tamper response */
    response.tag[0] ^= 0xFF;

    TEST_ASSERT_NOT_EQUAL(
        0,
        device_response_verify(&id, &challenge, &response)
    );
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_device_identity_generation);
    RUN_TEST(test_device_challenge_generation);
    RUN_TEST(test_device_response_verification_success);
    RUN_TEST(test_device_response_verification_fail_on_tamper);

    return UNITY_END();
}