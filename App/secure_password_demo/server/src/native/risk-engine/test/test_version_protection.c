#include <stdio.h>
#include <string.h>

#include "version.h"

/*
Test for User Story 1.9:
Version and Integrity Protection
*/

int main()
{
    PolicyMetadata meta;

    printf("=== Version Integrity Test (1.9) ===\n");

    /* 1. Initialize valid metadata */
    policy_metadata_init(&meta);

    if (policy_metadata_verify(&meta) == 0)
        printf("PASS: Valid policy metadata accepted\n");
    else
        printf("FAIL: Valid metadata rejected\n");

    /* 2. Simulate tampering (modify version) */
    meta.engine_version = 999;

    if (policy_metadata_verify(&meta) != 0)
        printf("PASS: Tampered version detected\n");
    else
        printf("FAIL: Tampered version NOT detected\n");

    /* 3. Reset and simulate hash tampering */
    policy_metadata_init(&meta);
    meta.integrity_hash[0] ^= 0xFF;

    if (policy_metadata_verify(&meta) != 0)
        printf("PASS: Integrity hash tampering detected\n");
    else
        printf("FAIL: Hash tampering NOT detected\n");

    return 0;
}