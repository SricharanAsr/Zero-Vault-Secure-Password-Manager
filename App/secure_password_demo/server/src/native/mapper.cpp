#include "mapper.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Global signal registry loaded at startup
std::unordered_map<std::string, uint16_t> signalMap;

void load_registry() {
    std::ifstream f("src/native/signal_registry.json");
    if (!f.is_open()) {
        std::cerr << "Warning: Could not open signal_registry.json" << std::endl;
        return; // Fallback to empty map
    }

    try {
        json data;
        f >> data;

        for (auto& el : data.items()) {
            if (el.value().is_number_integer()) {
                signalMap[el.key()] = el.value().get<uint16_t>();
            }
        }
        std::cout << "Successfully loaded " << signalMap.size() << " risk signals from registry." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing signal_registry.json: " << e.what() << std::endl;
    }
}

bool map_json_to_risk_input(const Napi::Object &obj, RiskInput *out, RiskInputHeader *headerOut, SignalEntry* signalsOut, size_t max_signals)
{

    // Prepare header
    headerOut->schema_version = 1;
    headerOut->engine_version = 1;
    headerOut->signal_bitmap_hash = 0; // The C engine calculates this or we can pass default, check evaluate docs

    size_t count = 0;
    
    // Iterate over JSON keys and map them to defined Signals
    Napi::Array props = obj.GetPropertyNames();
    for (uint32_t i = 0; i < props.Length(); i++) {
        if (count >= max_signals) break;

        Napi::Value keyVal = props[i];
        if (!keyVal.IsString()) continue;

        std::string key = keyVal.As<Napi::String>().Utf8Value();
        
        auto it = signalMap.find(key);
        if (it != signalMap.end()) {
            Napi::Value val = obj.Get(key);
            if (val.IsNumber()) {
                signalsOut[count].signal_id = it->second;
                signalsOut[count].normalized_value = val.As<Napi::Number>().Int64Value();
                count++;
            } else if (val.IsBoolean()) {
                signalsOut[count].signal_id = it->second;
                signalsOut[count].normalized_value = val.As<Napi::Boolean>().Value() ? 1 : 0;
                count++;
            }
        }
    }

    out->header = headerOut;
    out->signals = signalsOut;
    out->signal_count = count;

    return true;
}
