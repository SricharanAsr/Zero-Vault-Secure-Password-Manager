#include "unity/unity.h"
#include "input/hash.h"
#include "input/validate.h"
#include <string.h>

static RiskInputHeader header = {
    .schema_version = 1,
    .engine_version = 1,
    .signal_bitmap_hash = 0x1111
};

void setUp(void) {}
void tearDown(void) {}

void test_hash_determinism(void)
{
    RiskInput input = {
        .header = &header,
        .signals = NULL,
        .signal_count = 0
    };

    uint8_t h1[32], h2[32];

    compute_risk_input_hash(&input, h1);
    compute_risk_input_hash(&input, h2);

    TEST_ASSERT_EQUAL_MEMORY(h1, h2, 32);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hash_determinism);
    return UNITY_END();
}
