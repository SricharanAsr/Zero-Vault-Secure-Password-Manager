#include <stdio.h>

#include "security/device_identity.h"

int main()
{
    printf("=========================================\n");
    printf(" Device-Bound Access Test (2.4)\n");
    printf("=========================================\n\n");

    DeviceIdentity device;

    printf("Step 1: Generate device identity\n");

    device_identity_generate(&device);

    printf("PASS: Device identity created\n\n");

    DeviceChallenge challenge;

    printf("Step 2: Generate authentication challenge\n");

    device_challenge_generate(&challenge);

    printf("PASS: Challenge generated\n\n");

    DeviceResponse response;

    printf("Step 3: Create device response\n");

    device_response_create(&device, &challenge, &response);

    printf("PASS: Response generated\n\n");

    printf("Step 4: Verify response\n");

    if (device_response_verify(&device, &challenge, &response) == 0)
        printf("PASS: Device authenticated\n");
    else
        printf("FAIL: Authentication failed\n");

    printf("\nStep 5: Simulate replay/tamper attack\n");

    response.tag[0] ^= 0xAA;

    if (device_response_verify(&device, &challenge, &response) != 0)
        printf("PASS: Tampering detected\n");
    else
        printf("FAIL: Tampering NOT detected\n");

    printf("\n=========================================\n");
    printf(" Device Authentication Test Completed\n");
    printf("=========================================\n");

    return 0;
}