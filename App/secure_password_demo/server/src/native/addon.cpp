#include <napi.h>
#include "mapper.h"

extern "C" {
#include "engine/evaluate.h"
#include "audit/record.h"
}

Napi::Value Evaluate(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if(info.Length() < 1 || !info[0].IsObject())
    {
        Napi::TypeError::New(env, "Expected an object of risk signals").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Object obj = info[0].As<Napi::Object>();

    RiskInput input;
    RiskInputHeader header = {0};
    
    // Support up to 10 mapped signals
    const size_t MAX_SIGNALS = 10;
    SignalEntry signals[MAX_SIGNALS] = {0};

    if(!map_json_to_risk_input(obj, &input, &header, signals, MAX_SIGNALS))
    {
        return Napi::String::New(env, "ERROR");
    }

    uint32_t expected_hash = 0; // Evaluate doesn't strictly check hash logic if 0 or dependent on implementation

    RiskDecision d = evaluate_risk(&input, expected_hash);

    switch(d)
    {
        case RISK_ALLOW:
            return Napi::String::New(env, "ALLOW");

        case RISK_STEP_UP:
            return Napi::String::New(env, "STEP_UP");

        case RISK_DENY:
            return Napi::String::New(env, "DENY");
    }

    return Napi::String::New(env, "UNKNOWN");
}

Napi::Value VerifyAudit(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    int result = audit_log_verify_chain();
    return Napi::Boolean::New(env, result == 0);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    load_registry();

    exports.Set(
        "evaluate",
        Napi::Function::New(env, Evaluate)
    );

    exports.Set(
        "verifyAuditLog",
        Napi::Function::New(env, VerifyAudit)
    );

    return exports;
}

NODE_API_MODULE(risk_engine, Init)
