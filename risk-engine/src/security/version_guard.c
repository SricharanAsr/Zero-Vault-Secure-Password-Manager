#include "security/version_guard.h"
#include <stdio.h>

#define VERSION_FILE "policy_version.dat"

int version_guard_load(uint32_t *version)
{
    FILE *f = fopen(VERSION_FILE, "rb");

    if (!f)
    {
        *version = 0;
        return 0;
    }

    fread(version, sizeof(uint32_t), 1, f);
    fclose(f);

    return 0;
}

int version_guard_update(uint32_t new_version)
{
    FILE *f = fopen(VERSION_FILE, "wb");

    if (!f)
        return -1;

    fwrite(&new_version, sizeof(uint32_t), 1, f);
    fclose(f);

    return 0;
}