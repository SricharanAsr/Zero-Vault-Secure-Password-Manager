#ifndef RISK_INPUT_MAPPER_H
#define RISK_INPUT_MAPPER_H

#include <napi.h>

extern "C" {
#include "engine/evaluate.h"
#include "input/registry.h"
#include "input/signal.h"
#include "input/header.h"
}

// Loads the dynamic JSON signal mapping registry
void load_registry();

// Maps a Javascript Object representing dynamic signals 
// to a RiskInput Structure dynamically based on the SignalRegistry
bool map_json_to_risk_input(const Napi::Object &obj, RiskInput *out, RiskInputHeader *headerOut, SignalEntry* signalsOut, size_t max_signals);

#endif
