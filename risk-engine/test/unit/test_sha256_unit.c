#include "unity/unity.h"
#include "crypto/sha256.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_sha256_consistency(void)
{
    const uint8_t data[] = "abc";
    uint8_t h1[32], h2[32];

    Sha256Ctx c1, c2;

    sha256_init(&c1);
    sha256_update(&c1, data, 3);
    sha256_final(&c1, h1);

    sha256_init(&c2);
    sha256_update(&c2, data, 3);
    sha256_final(&c2, h2);

    TEST_ASSERT_EQUAL_MEMORY(h1, h2, 32);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_sha256_consistency);
    return UNITY_END();
}
