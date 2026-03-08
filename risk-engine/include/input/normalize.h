#ifndef INPUT_NORMALIZE_H
#define INPUT_NORMALIZE_H

#include "registry.h"

int normalize_signal(const SignalMeta* meta, int64_t raw, int64_t* out);

#endif
