#include <stdio.h>
#include <stdint.h>

#include "engine/evaluate.h"
#include "input/validate.h"
#include "decision.h"

/* Example signal bitmap hash (must match your registry) */
#define EXPECTED_HASH 0xDEADBEEF

int main(void) {
    /* Header */
    RiskInputHeader header = {
        .schema_version = 1,
        .engine_version = 1,
        .signal_bitmap_hash = EXPECTED_HASH
    };

    /* Signals MUST be sorted by signal_id */
    SignalEntry signals[] = {
        { .signal_id = 0x0001, .normalized_value = 1 }, /* Secure boot */
        { .signal_id = 0x0002, .normalized_value = 0 }, /* Failures */
        { .signal_id = 0x0003, .normalized_value = 1 }  /* Trusted device */
    };

    RiskInput input = {
        .header = &header,
        .signals = signals,
        .signal_count = 3
    };

    RiskDecision decision = evaluate_risk(&input, EXPECTED_HASH);

    printf("Risk decision = %d\n", decision);

    return 0;
}
