/*
 * User Story 1.5 Test:
 * Centralized Authentication Entry Points
 *
 * This file intentionally attempts to bypass the risk engine.
 * The test PASSES if this file FAILS to compile or link.
 */

#include "decision.h"

/* Attempt to declare a fake alternate entry point */
RiskDecision bypass_decision(void)
{
    /* There should be NO way to construct a decision directly */
    return RISK_ALLOW;
}


// command to test in mysys2 : gcc -std=c11 -Iinclude test/test_centralized_entry.c -o bypass_test.exe
