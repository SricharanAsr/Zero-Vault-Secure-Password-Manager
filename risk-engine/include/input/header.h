#ifndef INPUT_HEADER_H
#define INPUT_HEADER_H

#include <stdint.h>

typedef struct {
    uint16_t schema_version;
    uint16_t engine_version;
    uint32_t signal_bitmap_hash;
} RiskInputHeader;

#endif
