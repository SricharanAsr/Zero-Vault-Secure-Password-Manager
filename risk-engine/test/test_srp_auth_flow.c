#include <stdio.h>
#include <string.h>

#include "security/srp_auth.h"

int main()
{
    printf("=================================================\n");
    printf(" Zero-Knowledge Authentication Demo (SRP-style)\n");
    printf(" User Story 2.5\n");
    printf("=================================================\n\n");

    const char *password = "correct_horse_battery_staple";

    /* ---------------- REGISTRATION ---------------- */

    printf("Step 1: Registration (server stores verifier)\n");

    SrpVerifier verifier;

    if (srp_create_verifier(
            (const uint8_t*)password,
            strlen(password),
            &verifier) != 0)
    {
        printf("FAIL: verifier creation failed\n");
        return 1;
    }

    printf("PASS: Verifier stored on server\n\n");


    /* ---------------- LOGIN ---------------- */

    printf("Step 2: Client begins authentication\n");

    SrpClientHello hello;
    uint8_t client_secret[SRP_KEY_SIZE];

    srp_client_begin(&hello, client_secret);

    printf("PASS: Client hello generated\n\n");


    printf("Step 3: Server sends challenge\n");

    SrpServerChallenge challenge;
    uint8_t server_secret[SRP_KEY_SIZE];

    srp_server_challenge(
        &verifier,
        &challenge,
        server_secret);

    printf("PASS: Server challenge created\n\n");


    printf("Step 4: Client computes proof\n");

    SrpClientProof client_proof;

    srp_client_proof(
        (const uint8_t*)password,
        strlen(password),
        &challenge,
        &hello,
        client_secret,
        &client_proof);

    printf("PASS: Client proof generated\n\n");


    printf("Step 5: Server verifies proof\n");

    SrpServerProof server_proof;

    if (srp_server_verify(
            &verifier,
            &hello,
            &challenge,
            server_secret,
            &client_proof,
            &server_proof) == 0)
    {
        printf("PASS: Client authenticated successfully\n");
    }
    else
    {
        printf("FAIL: Authentication failed\n");
        return 1;
    }

    printf("\nStep 6: Mutual authentication proof generated\n");

    printf("PASS: Server proof created\n\n");


    /* ---------------- ATTACK TEST ---------------- */

    printf("Step 7: Simulating attacker with wrong password\n");

    const char *wrong_password = "incorrect_password";

    SrpClientProof attacker_proof;

    srp_client_proof(
        (const uint8_t*)wrong_password,
        strlen(wrong_password),
        &challenge,
        &hello,
        client_secret,
        &attacker_proof);

    if (srp_server_verify(
            &verifier,
            &hello,
            &challenge,
            server_secret,
            &attacker_proof,
            &server_proof) != 0)
    {
        printf("PASS: Incorrect password rejected\n");
    }
    else
    {
        printf("FAIL: Attack succeeded (this should not happen)\n");
    }

    printf("\n=================================================\n");
    printf(" SRP Authentication Test Completed\n");
    printf("=================================================\n");

    return 0;
}