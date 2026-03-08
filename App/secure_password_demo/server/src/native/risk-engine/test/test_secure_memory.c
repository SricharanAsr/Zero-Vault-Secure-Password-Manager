#include <stdio.h>
#include <string.h>

#include "security/secure_memory.h"

int main()
{
    printf("=========================================\n");
    printf(" Secure Memory Lifecycle Test (2.3)\n");
    printf("=========================================\n\n");

    SecureBuffer key;

    printf("Step 1: Allocate secure memory\n");

    if (secure_alloc(&key, 32) != 0)
    {
        printf("FAIL: Allocation failed\n");
        return 1;
    }

    printf("PASS: Secure memory allocated\n");

    strcpy((char*)key.data, "temporary_secret_key");

    printf("Stored key: %s\n", key.data);

    printf("\nStep 2: Free secure memory\n");

    secure_free(&key);

    if (!key.active && key.data == NULL)
        printf("PASS: Key securely erased and freed\n");
    else
        printf("FAIL: Key lifecycle error\n");

    printf("\n=========================================\n");
    printf(" Secure Memory Test Completed\n");
    printf("=========================================\n");

    return 0;
}