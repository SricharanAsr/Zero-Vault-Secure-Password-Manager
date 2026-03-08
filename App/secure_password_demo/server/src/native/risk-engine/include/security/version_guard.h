#ifndef VERSION_GUARD_H
#define VERSION_GUARD_H

#include <stdint.h>

int version_guard_load(uint32_t *version);
int version_guard_update(uint32_t new_version);

#endif