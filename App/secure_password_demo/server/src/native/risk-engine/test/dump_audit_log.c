#include <stdio.h>
#include <stdint.h>

#include "audit/record.h"

static const char* decision_to_string(uint16_t d)
{
    switch (d) {
        case 0: return "ALLOW";
        case 1: return "STEP_UP";
        case 2: return "DENY";
        default: return "UNKNOWN";
    }
}

int main(void)
{
    FILE* f = fopen("audit.log", "rb");
    if (!f) {
        perror("audit.log");
        return 1;
    }

    AuditRecord rec;

    while (fread(&rec, sizeof(rec), 1, f) == 1) {

        printf("Record #%llu\n",
            (unsigned long long)rec.record_index);

        printf("  Engine version : %u\n", rec.engine_version);
        printf("  Decision       : %s\n",
            decision_to_string(rec.decision));

        printf("  Input hash     : ");
        for (int i = 0; i < 8; i++)
            printf("%02x", rec.risk_input_hash[i]);
        printf("...\n");

        printf("  Record hash    : ");
        for (int i = 0; i < 8; i++)
            printf("%02x", rec.record_hash[i]);
        printf("...\n");

        printf("\n");
    }

    fclose(f);
    return 0;
}
