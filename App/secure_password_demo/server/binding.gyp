{
  "targets": [
    {
      "target_name": "risk_engine",
      "sources": [
        "src/native/addon.cpp",
        "src/native/mapper.cpp",
        "src/native/risk-engine/src/audit/log.c",
        "src/native/risk-engine/src/crypto/sha256.c",
        "src/native/risk-engine/src/engine/evaluate.c",
        "src/native/risk-engine/src/engine/version.c",
        "src/native/risk-engine/src/input/hash.c",
        "src/native/risk-engine/src/input/normalize.c",
        "src/native/risk-engine/src/input/validate.c",
        "src/native/risk-engine/src/rules/device.c",
        "src/native/risk-engine/src/rules/failures.c",
        "src/native/risk-engine/src/rules/integrity.c",
        "src/native/risk-engine/src/security/device_identity.c",
        "src/native/risk-engine/src/security/kdf.c",
        "src/native/risk-engine/src/security/metadata_crypto.c",
        "src/native/risk-engine/src/security/policy_verify.c",
        "src/native/risk-engine/src/security/secure_memory.c",
        "src/native/risk-engine/src/security/srp_auth.c",
        "src/native/risk-engine/src/security/vault_crypto.c",
        "src/native/risk-engine/src/security/version_guard.c"
      ],
      "include_dirs": [
        "src/native/risk-engine/include",
        "src/native/include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "libraries": [],
      "cflags_cc": [ "-std=c++17", "-fvisibility=hidden" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "conditions": [
        ['OS=="mac"', {
          "xcode_settings": {
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
            "CLANG_CXX_LIBRARY": "libc++",
            "MACOSX_DEPLOYMENT_TARGET": "10.7"
          }
        }],
        ['OS=="win"', {
          "msvs_settings": {
            "VCCLCompilerTool": {
               "ExceptionHandling": 1
            }
          }
        }]
      ]
    }
  ]
}
