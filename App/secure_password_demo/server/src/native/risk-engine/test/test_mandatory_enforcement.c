#include <stdio.h>
#include <assert.h>
#include <direct.h>
#include "engine/evaluate.h"
#include "input/validate.h"
#include "decision.h"

/*
 * Test for User Story 1.4:
 * Mandatory Risk Engine Enforcement (No Bypass)
 *
 * Strategy:
 * - Break a mandatory enforcement stage (audit logging)
 * - Prove that authentication is denied
 */

/* Valid input generator */
static RiskInput make_valid_input(void)
{
    static RiskInputHeader header = {
        .schema_version = 1,
        .engine_version = 1,
        .signal_bitmap_hash = 0x11111111
    };

    RiskInput input = {
        .header = &header,
        .signals = NULL,
        .signal_count = 0
    };

    return input;
}

int main(void)
{
    RiskInput input = make_valid_input();

    /* Baseline: normal enforcement path */
    RiskDecision d1 = evaluate_risk(&input, 0);
    assert(d1 == RISK_ALLOW || d1 == RISK_DENY || d1 == RISK_STEP_UP);

    /*
     * Break mandatory enforcement:
     * Replace audit.log with a directory
     */
    remove("audit.log");
    mkdir("audit.log");

    RiskDecision d2 = evaluate_risk(&input, 0);

    /* Mandatory enforcement => logging failure must deny */
    assert(d2 == RISK_DENY);

    /* Cleanup */
    rmdir("audit.log");

    printf("Mandatory enforcement test PASSED (no bypass possible)\n");
    return 0;
}
