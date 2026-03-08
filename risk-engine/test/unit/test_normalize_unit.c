#include "unity/unity.h"
#include "input/normalize.h"
#include <stdint.h>

void setUp(void) {}
void tearDown(void) {}

/*
 * Contract: NULL metadata must be rejected
 */
void test_normalize_rejects_null_meta(void)
{
    int64_t out = 0;
    int rc = normalize_signal(NULL, 10, &out);
    TEST_ASSERT_NOT_EQUAL(0, rc);
}

/*
 * Contract: NULL output pointer must be rejected
 */
void test_normalize_rejects_null_output(void)
{
    /* We cannot construct a valid SignalMeta here, so we use
       a non-NULL dummy pointer to test output validation */
    const SignalMeta* dummy_meta = (const SignalMeta*)0x1;

    int rc = normalize_signal(dummy_meta, 10, NULL);
    TEST_ASSERT_NOT_EQUAL(0, rc);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_normalize_rejects_null_meta);
    RUN_TEST(test_normalize_rejects_null_output);

    return UNITY_END();
}
