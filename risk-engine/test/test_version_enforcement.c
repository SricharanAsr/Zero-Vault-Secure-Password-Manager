#include <stdio.h>
#include <string.h>

#include "engine/evaluate.h"
#include "version.h"
#include "decision.h"

/*
Test for User Story 1.9 enforcement inside evaluate_risk()
*/

int main()
{
    printf("=== Risk Engine Version Enforcement Test ===\n");

    PolicyMetadata meta;

    /* Step 1 — valid metadata */
    policy_metadata_init(&meta);

    if (policy_metadata_verify(&meta) == 0)
        printf("PASS: Valid policy accepted\n");
    else
        printf("FAIL: Valid policy rejected\n");

    /* Step 2 — simulate rollback attack */
    meta.engine_version = 0;

    if (policy_metadata_verify(&meta) != 0)
        printf("PASS: Rollback attempt detected\n");
    else
        printf("FAIL: Rollback NOT detected\n");

    /* Step 3 — simulate integrity tampering */
    policy_metadata_init(&meta);
    meta.integrity_hash[3] ^= 0xAA;

    if (policy_metadata_verify(&meta) != 0)
        printf("PASS: Integrity tampering detected\n");
    else
        printf("FAIL: Tampering NOT detected\n");

    return 0;
}