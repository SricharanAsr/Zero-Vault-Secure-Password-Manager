#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "engine/evaluate.h"
#include "input/hash.h"
#include "input/validate.h"
#include "decision.h"


/* Create a fixed, deterministic RiskInput */
static RiskInput make_fixed_input(void)
{
    static RiskInputHeader header = {
        .schema_version = 1,
        .engine_version = 1,
        .signal_bitmap_hash = 0xABCD1234
    };

    static SignalEntry signals[] = {
        { .signal_id = 1, .normalized_value = 10 },
        { .signal_id = 2, .normalized_value = 20 }
    };

    RiskInput input = {
        .header = &header,
        .signals = signals,
        .signal_count = 2
    };

    return input;
}

int main(void)
{
    RiskInput input = make_fixed_input();

    uint8_t hash1[32];
    uint8_t hash2[32];

    /* Run #1 */
    RiskDecision d1 = evaluate_risk(&input, 0);
    compute_risk_input_hash(&input, hash1);

    /* Run #2 */
    RiskDecision d2 = evaluate_risk(&input, 0);
    compute_risk_input_hash(&input, hash2);

    /* Determinism checks */
    assert(d1 == d2);
    assert(memcmp(hash1, hash2, sizeof(hash1)) == 0);

    printf("Determinism test PASSED\n");
    printf("Decision: %d\n", d1);

    return 0;
}
