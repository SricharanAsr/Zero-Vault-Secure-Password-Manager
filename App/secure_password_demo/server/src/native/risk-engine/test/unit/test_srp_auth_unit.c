#include "unity/unity.h"
#include "security/srp_auth.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ---------- Test: verifier creation ---------- */

void test_srp_verifier_creation(void)
{
    const char *password = "secure_password";

    SrpVerifier verifier;

    TEST_ASSERT_EQUAL(
        0,
        srp_create_verifier(
            (const uint8_t*)password,
            strlen(password),
            &verifier)
    );

    int zero = 1;

    for (int i = 0; i < SRP_KEY_SIZE; i++)
    {
        if (verifier.verifier[i] != 0)
        {
            zero = 0;
            break;
        }
    }

    TEST_ASSERT_FALSE(zero);
}


/* ---------- Test: successful authentication ---------- */

void test_srp_auth_success(void)
{
    const char *password = "correct_password";

    SrpVerifier verifier;

    srp_create_verifier(
        (const uint8_t*)password,
        strlen(password),
        &verifier);

    SrpClientHello hello;
    uint8_t client_secret[SRP_KEY_SIZE];

    srp_client_begin(&hello, client_secret);

    SrpServerChallenge challenge;
    uint8_t server_secret[SRP_KEY_SIZE];

    srp_server_challenge(
        &verifier,
        &challenge,
        server_secret);

    SrpClientProof proof;

    srp_client_proof(
        (const uint8_t*)password,
        strlen(password),
        &challenge,
        &hello,
        client_secret,
        &proof);

    SrpServerProof response;

    TEST_ASSERT_EQUAL(
        0,
        srp_server_verify(
            &verifier,
            &hello,
            &challenge,
            server_secret,
            &proof,
            &response)
    );
}


/* ---------- Test: wrong password rejected ---------- */

void test_srp_wrong_password(void)
{
    const char *password = "correct_password";
    const char *wrong = "incorrect";

    SrpVerifier verifier;

    srp_create_verifier(
        (const uint8_t*)password,
        strlen(password),
        &verifier);

    SrpClientHello hello;
    uint8_t client_secret[SRP_KEY_SIZE];

    srp_client_begin(&hello, client_secret);

    SrpServerChallenge challenge;
    uint8_t server_secret[SRP_KEY_SIZE];

    srp_server_challenge(
        &verifier,
        &challenge,
        server_secret);

    SrpClientProof proof;

    srp_client_proof(
        (const uint8_t*)wrong,
        strlen(wrong),
        &challenge,
        &hello,
        client_secret,
        &proof);

    SrpServerProof response;

    TEST_ASSERT_NOT_EQUAL(
        0,
        srp_server_verify(
            &verifier,
            &hello,
            &challenge,
            server_secret,
            &proof,
            &response)
    );
}


/* ---------- Test: tampered proof detection ---------- */

void test_srp_proof_tampering(void)
{
    const char *password = "correct_password";

    SrpVerifier verifier;

    srp_create_verifier(
        (const uint8_t*)password,
        strlen(password),
        &verifier);

    SrpClientHello hello;
    uint8_t client_secret[SRP_KEY_SIZE];

    srp_client_begin(&hello, client_secret);

    SrpServerChallenge challenge;
    uint8_t server_secret[SRP_KEY_SIZE];

    srp_server_challenge(
        &verifier,
        &challenge,
        server_secret);

    SrpClientProof proof;

    srp_client_proof(
        (const uint8_t*)password,
        strlen(password),
        &challenge,
        &hello,
        client_secret,
        &proof);

    /* tamper proof */
    proof.proof[0] ^= 0xAA;

    SrpServerProof response;

    TEST_ASSERT_NOT_EQUAL(
        0,
        srp_server_verify(
            &verifier,
            &hello,
            &challenge,
            server_secret,
            &proof,
            &response)
    );
}


/* ---------- Unity runner ---------- */

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_srp_verifier_creation);
    RUN_TEST(test_srp_auth_success);
    RUN_TEST(test_srp_wrong_password);
    RUN_TEST(test_srp_proof_tampering);

    return UNITY_END();
}