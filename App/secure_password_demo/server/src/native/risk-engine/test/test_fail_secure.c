#include <stdio.h>
#include <assert.h>

#include "engine/evaluate.h"
#include "input/validate.h"
#include "decision.h"

/* Case 1: NULL input */
static void test_null_input(void)
{
    RiskDecision d = evaluate_risk(NULL, 0);
    assert(d == RISK_DENY);
}

/* Case 2: Header missing */
static void test_missing_header(void)
{
    RiskInput input = {
        .header = NULL,
        .signals = NULL,
        .signal_count = 0
    };

    RiskDecision d = evaluate_risk(&input, 0);
    assert(d == RISK_DENY);
}

/* Case 3: Signal count mismatch */
static void test_invalid_signal_count(void)
{
    static RiskInputHeader header = {
        .schema_version = 1,
        .engine_version = 1,
        .signal_bitmap_hash = 0x12345678
    };

    RiskInput input = {
        .header = &header,
        .signals = NULL,   /* invalid */
        .signal_count = 2  /* claims signals exist */
    };

    RiskDecision d = evaluate_risk(&input, 0);
    assert(d == RISK_DENY);
}

/* Case 4: Invalid expected hash */
static void test_hash_mismatch(void)
{
    static RiskInputHeader header = {
        .schema_version = 1,
        .engine_version = 1,
        .signal_bitmap_hash = 0x99999999
    };

    RiskInput input = {
        .header = &header,
        .signals = NULL,
        .signal_count = 0
    };

    /* Pass a deliberately wrong expected hash */
    RiskDecision d = evaluate_risk(&input, 0xDEADBEEF);
    assert(d == RISK_DENY);
}

int main(void)
{
    test_null_input();
    test_missing_header();
    test_invalid_signal_count();
    test_hash_mismatch();

    printf("Fail-secure tests PASSED (all failures denied)\n");
    return 0;
}
