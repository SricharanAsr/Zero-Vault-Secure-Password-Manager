#ifndef INPUT_REGISTRY_H
#define INPUT_REGISTRY_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t id;
    int64_t  min;
    int64_t  max;
    bool     required;
} SignalMeta;

#define SIGNAL_SECURE_BOOT     0x0001
#define SIGNAL_FAILURE_COUNT   0x0002
#define SIGNAL_DEVICE_TRUSTED  0x0003

static const SignalMeta SIGNAL_REGISTRY[] = {
    { SIGNAL_SECURE_BOOT,    0, 1,  true },
    { SIGNAL_FAILURE_COUNT,  0, 10, true },
    { SIGNAL_DEVICE_TRUSTED, 0, 1,  true }
};

#define SIGNAL_REGISTRY_COUNT \
    (sizeof(SIGNAL_REGISTRY) / sizeof(SIGNAL_REGISTRY[0]))

#endif
