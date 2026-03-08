#include <stdio.h>
#include <string.h>

#include "security/metadata_crypto.h"

int main()
{
    printf("=========================================\n");
    printf(" Metadata Protection Test (User Story 1.10)\n");
    printf("=========================================\n\n");

    RiskInputHeader header = {
        .schema_version = 1,
        .engine_version = 1,
        .signal_bitmap_hash = 1234
    };

    ProtectedMetadata encrypted;
    RiskInputHeader decrypted;

    printf("Step 1: Encrypt metadata\n");

    if (metadata_encrypt(&header, &encrypted) != 0) {
        printf("FAIL: Metadata encryption failed\n");
        return 1;
    }

    printf("PASS: Metadata encrypted successfully\n\n");

    printf("Step 2: Decrypt metadata\n");

    if (metadata_decrypt(&encrypted, &decrypted) != 0) {
        printf("FAIL: Metadata decryption failed\n");
        return 1;
    }

    if (memcmp(&header, &decrypted, sizeof(RiskInputHeader)) == 0)
        printf("PASS: Metadata decrypted correctly\n\n");
    else
        printf("FAIL: Decrypted metadata does not match original\n\n");

    printf("Step 3: Simulate tampering attack\n");

    encrypted.ciphertext[0] ^= 0xAA;

    if (metadata_decrypt(&encrypted, &decrypted) != 0)
        printf("PASS: Tampering detected (authentication failed)\n");
    else
        printf("FAIL: Tampering NOT detected\n");

    printf("\n=========================================\n");
    printf(" Metadata Protection Test Completed\n");
    printf("=========================================\n");

    return 0;
}