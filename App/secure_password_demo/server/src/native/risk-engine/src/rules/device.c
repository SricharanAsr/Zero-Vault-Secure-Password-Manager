#include "rules/device.h"

RiskDecision device_rule(const RiskInput* input) {
    for (size_t i = 0; i < input->signal_count; i++) {
        if (input->signals[i].signal_id == SIGNAL_DEVICE_TRUSTED) {
            return input->signals[i].normalized_value ? RISK_ALLOW : RISK_STEP_UP;
        }
    }
    return RISK_STEP_UP;
}
