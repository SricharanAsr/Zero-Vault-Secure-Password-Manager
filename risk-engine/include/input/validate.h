#ifndef INPUT_VALIDATE_H
#define INPUT_VALIDATE_H

#include "header.h"
#include "signal.h"
#include "registry.h"
#include "error.h"
#include <stddef.h>

typedef struct {
    const RiskInputHeader* header;
    const SignalEntry*     signals;
    size_t                 signal_count;
} RiskInput;

RiskError validate_risk_input(const RiskInput* input, uint32_t expected_hash);

#endif
