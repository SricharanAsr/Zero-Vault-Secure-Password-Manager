#ifndef INPUT_HASH_H
#define INPUT_HASH_H

#include <stdint.h>
#include "input/validate.h"

#define RISK_INPUT_HASH_SIZE 32  

int compute_risk_input_hash(
    const RiskInput* input,
    uint8_t out[RISK_INPUT_HASH_SIZE]
);

#endif
