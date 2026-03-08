#include "rules/failures.h"

RiskDecision failure_rule(const RiskInput* input) {
    for (size_t i = 0; i < input->signal_count; i++) {
        if (input->signals[i].signal_id == SIGNAL_FAILURE_COUNT) {
            if (input->signals[i].normalized_value >= 5) {
                return RISK_DENY;
            }
            if (input->signals[i].normalized_value > 0) {
                return RISK_STEP_UP;
            }
            return RISK_ALLOW;
        }
    }
    return RISK_DENY;
}
