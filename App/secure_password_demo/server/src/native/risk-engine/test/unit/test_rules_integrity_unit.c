#include "unity/unity.h"
#include "rules/integrity.h"
#include "input/validate.h"

static RiskInputHeader bad_header = {
    .schema_version = 0,
    .engine_version = 0,
    .signal_bitmap_hash = 0
};

void setUp(void) {}
void tearDown(void) {}

void test_integrity_rule_invalid_header(void)
{
    RiskInput input = {
        .header = &bad_header,
        .signals = NULL,
        .signal_count = 0
    };

    TEST_ASSERT_EQUAL(RISK_DENY, integrity_rule(&input));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_integrity_rule_invalid_header);
    return UNITY_END();
}
