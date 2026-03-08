#include "unity/unity.h"
#include "security/secure_memory.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_secure_alloc_initializes_memory(void)
{
    SecureBuffer buf;

    TEST_ASSERT_EQUAL(0, secure_alloc(&buf, 32));

    TEST_ASSERT_NOT_NULL(buf.data);
    TEST_ASSERT_EQUAL(32, buf.length);
    TEST_ASSERT_TRUE(buf.active);

    /* memory should start zeroed */
    for (size_t i = 0; i < buf.length; i++)
        TEST_ASSERT_EQUAL_UINT8(0, buf.data[i]);

    secure_free(&buf);
}

void test_secure_buffer_writable(void)
{
    SecureBuffer buf;

    secure_alloc(&buf, 32);

    const char *secret = "temporary_key";

    memcpy(buf.data, secret, strlen(secret));

    TEST_ASSERT_EQUAL_MEMORY(secret, buf.data, strlen(secret));

    secure_free(&buf);
}

void test_secure_free_invalidates_buffer(void)
{
    SecureBuffer buf;

    secure_alloc(&buf, 32);

    memcpy(buf.data, "secret", 6);

    secure_free(&buf);

    TEST_ASSERT_NULL(buf.data);
    TEST_ASSERT_EQUAL(0, buf.length);
    TEST_ASSERT_FALSE(buf.active);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_secure_alloc_initializes_memory);
    RUN_TEST(test_secure_buffer_writable);
    RUN_TEST(test_secure_free_invalidates_buffer);

    return UNITY_END();
}