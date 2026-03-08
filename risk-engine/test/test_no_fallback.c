#include <stdio.h>
#include <assert.h>

#include "engine/evaluate.h"
#include "input/validate.h"
#include "decision.h"

/*
 * User Story 1.6 Test:
 * Single Enforcement Path (No Weak Fallbacks)
 *
 * Strategy:
 * - Feed an input that deterministically causes DENY
 * - Verify DENY is final and cannot be overridden
 */

static RiskInput make_deny_input(void)
{
    /* Intentionally invalid header to force DENY */
    static RiskInputHeader bad_header = {
        .schema_version = 0,  /* invalid */
        .engine_version = 0,
        .signal_bitmap_hash = 0
    };

    RiskInput input = {
        .header = &bad_header,
        .signals = NULL,
        .signal_count = 0
    };

    return input;
}

int main(void)
{
    RiskInput input = make_deny_input();

    /* First evaluation */
    RiskDecision d1 = evaluate_risk(&input, 0);
    assert(d1 == RISK_DENY);

    /* Second evaluation (no fallback, no recovery) */
    RiskDecision d2 = evaluate_risk(&input, 0);
    assert(d2 == RISK_DENY);

    printf("No-fallback test PASSED (DENY is final)\n");
    return 0;
}
