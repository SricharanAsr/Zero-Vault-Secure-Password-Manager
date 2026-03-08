#ifndef INPUT_SIGNAL_H
#define INPUT_SIGNAL_H

#include <stdint.h>

typedef struct {
    uint16_t signal_id;
    int64_t  normalized_value;
} SignalEntry;

#endif
