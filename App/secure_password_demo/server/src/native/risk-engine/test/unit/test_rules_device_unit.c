#include "unity/unity.h"
#include "rules/device.h"
#include "input/validate.h"

static RiskInputHeader minimal_header = {
    .schema_version = 1,
    .engine_version = 1,
    .signal_bitmap_hash = 0
};

static RiskInput minimal_input = {
    .header = &minimal_header,
    .signals = NULL,
    .signal_count = 0
};

void setUp(void) {}
void tearDown(void) {}

/*
 * Contract:
 * device_rule operates on validated, non-NULL input.
 * This test ensures it returns a valid RiskDecision
 * and does not crash for minimal valid input.
 */
void test_device_rule_minimal_input_is_safe(void)
{
    RiskDecision d = device_rule(&minimal_input);

    TEST_ASSERT_TRUE(
        d == RISK_ALLOW ||
        d == RISK_STEP_UP ||
        d == RISK_DENY
    );
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_device_rule_minimal_input_is_safe);
    return UNITY_END();
}
