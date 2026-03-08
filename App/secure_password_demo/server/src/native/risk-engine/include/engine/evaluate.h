#ifndef ENGINE_EVALUATE_H
#define ENGINE_EVALUATE_H

#include "decision.h"
#include "input/validate.h"

RiskDecision evaluate_risk(const RiskInput* input, uint32_t expected_hash);

#endif
