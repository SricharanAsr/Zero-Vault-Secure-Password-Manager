#include "unity/unity.h"
#include "input/validate.h"

static RiskInputHeader some_header = {
    .schema_version = 1,
    .engine_version = 1,
    .signal_bitmap_hash = 0x1234
};

static SignalEntry some_signals[] = {
    { .signal_id = 1, .normalized_value = 10 }
};

void setUp(void) {}
void tearDown(void) {}

/*
 * Contract: NULL input must be rejected
 */
void test_validate_null_input_is_rejected(void)
{
    int rc = validate_risk_input(NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, rc);
}

/*
 * Contract: structurally incomplete input must be rejected
 */
void test_validate_missing_header_is_rejected(void)
{
    RiskInput input = {
        .header = NULL,
        .signals = some_signals,
        .signal_count = 1
    };

    int rc = validate_risk_input(&input, 0);
    TEST_ASSERT_NOT_EQUAL(0, rc);
}

/*
 * Contract: validator must not accept partially-constructed inputs
 * We do NOT assert acceptance here because full validity requires
 * canonical engine-owned construction.
 */
void test_validate_partially_constructed_input_is_rejected(void)
{
    RiskInput input = {
        .header = &some_header,
        .signals = some_signals,
        .signal_count = 1
    };

    int rc = validate_risk_input(&input, 0);
    TEST_ASSERT_NOT_EQUAL(0, rc);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_validate_null_input_is_rejected);
    RUN_TEST(test_validate_missing_header_is_rejected);
    RUN_TEST(test_validate_partially_constructed_input_is_rejected);

    return UNITY_END();
}
