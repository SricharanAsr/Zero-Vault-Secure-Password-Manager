#include "input/validate.h"
#include "version.h"

RiskError validate_risk_input(const RiskInput* input, uint32_t expected_hash) {
    if (!input || !input->header) {
        return RISK_ERR_INVALID_HEADER;
    }

    if (input->header->schema_version != RISK_SCHEMA_VERSION ||
        input->header->engine_version != RISK_ENGINE_VERSION ||
        input->header->signal_bitmap_hash != expected_hash) {
        return RISK_ERR_VERSION_MISMATCH;
    }

    uint16_t last_id = 0;
    for (size_t i = 0; i < input->signal_count; i++) {
        uint16_t id = input->signals[i].signal_id;
        if (id <= last_id) {
            return RISK_ERR_INVALID_ORDER;
        }
        last_id = id;
    }

    for (size_t m = 0; m < SIGNAL_REGISTRY_COUNT; m++) {
        bool found = false;
        for (size_t i = 0; i < input->signal_count; i++) {
            if (input->signals[i].signal_id == SIGNAL_REGISTRY[m].id) {
                found = true;
                break;
            }
        }
        if (SIGNAL_REGISTRY[m].required && !found) {
            return RISK_ERR_MISSING_SIGNAL;
        }
    }

    return RISK_OK;
}
