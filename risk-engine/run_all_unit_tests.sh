#!/bin/sh
set -e

CC=gcc
CFLAGS="-std=c11 -Iinclude -Itest"
UNITY_SRC="test/unity/unity.c"

TOTAL_TESTS=0

echo "============================================================"
echo " RISK ENGINE – UNIT TEST EXECUTION (Unity)"
echo "============================================================"
echo

run_test () {
    NAME="$1"
    STORY="$2"
    shift 2

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    echo "------------------------------------------------------------"
    echo " TEST: $NAME   |   User Story: $STORY"
    echo "------------------------------------------------------------"

    "$@"

    echo "------------------------------------------------------------"
    echo " RESULT: $NAME PASSED"
    echo "------------------------------------------------------------"
    echo
}

# ============================================================
# EPIC 1 — Adaptive Risk-Based Authentication
# ============================================================

run_test "validate_risk_input" "1.1" \
$CC $CFLAGS \
test/unit/test_validate_unit.c \
$UNITY_SRC \
src/input/validate.c \
-o test_validate_unit.exe && \
./test_validate_unit.exe


run_test "normalize_signal" "1.1" \
$CC $CFLAGS \
test/unit/test_normalize_unit.c \
$UNITY_SRC \
src/input/normalize.c \
-o test_normalize_unit.exe && \
./test_normalize_unit.exe


run_test "compute_risk_input_hash" "1.2" \
$CC $CFLAGS \
test/unit/test_hash_unit.c \
$UNITY_SRC \
src/input/hash.c \
src/crypto/sha256.c \
-o test_hash_unit.exe && \
./test_hash_unit.exe


run_test "sha256_core" "1.2" \
$CC $CFLAGS \
test/unit/test_sha256_unit.c \
$UNITY_SRC \
src/crypto/sha256.c \
-o test_sha256_unit.exe && \
./test_sha256_unit.exe


run_test "failure_rule" "1.3" \
$CC $CFLAGS \
test/unit/test_rules_failures_unit.c \
$UNITY_SRC \
src/rules/failures.c \
-o test_rules_failures_unit.exe && \
./test_rules_failures_unit.exe


run_test "integrity_rule" "1.4 / 1.5" \
$CC $CFLAGS \
test/unit/test_rules_integrity_unit.c \
$UNITY_SRC \
src/rules/integrity.c \
-o test_rules_integrity_unit.exe && \
./test_rules_integrity_unit.exe


run_test "device_rule" "1.6" \
$CC $CFLAGS \
test/unit/test_rules_device_unit.c \
$UNITY_SRC \
src/rules/device.c \
-o test_rules_device_unit.exe && \
./test_rules_device_unit.exe


run_test "audit_log_append_only" "1.7" \
$CC $CFLAGS \
test/unit/test_audit_log_unit.c \
$UNITY_SRC \
src/audit/log.c \
src/crypto/sha256.c \
-o test_audit_log_unit.exe && \
./test_audit_log_unit.exe


run_test "audit_hash_chain_integrity" "1.8" \
$CC $CFLAGS \
test/unit/test_audit_chain_unit.c \
$UNITY_SRC \
src/audit/log.c \
src/crypto/sha256.c \
-o test_audit_chain_unit.exe && \
./test_audit_chain_unit.exe


run_test "version_integrity_protection" "1.9" \
$CC $CFLAGS \
test/unit/test_version_unit.c \
$UNITY_SRC \
src/engine/version.c \
src/crypto/sha256.c \
-o test_version_unit.exe && \
./test_version_unit.exe


run_test "metadata_crypto_protection" "1.10" \
$CC $CFLAGS \
test/unit/test_metadata_crypto_unit.c \
$UNITY_SRC \
src/security/metadata_crypto.c \
src/crypto/sha256.c \
-o test_metadata_crypto_unit.exe && \
./test_metadata_crypto_unit.exe



# ============================================================
# EPIC 2 — Zero-Knowledge Vault Encryption
# ============================================================

run_test "vault_encryption" "2.1" \
$CC $CFLAGS \
test/unit/test_vault_crypto_unit.c \
$UNITY_SRC \
src/security/vault_crypto.c \
src/crypto/sha256.c \
-o test_vault_crypto_unit.exe && \
./test_vault_crypto_unit.exe


run_test "key_derivation_kdf" "2.2" \
$CC $CFLAGS \
test/unit/test_kdf_unit.c \
$UNITY_SRC \
src/security/kdf.c \
src/crypto/sha256.c \
-o test_kdf_unit.exe && \
./test_kdf_unit.exe


run_test "secure_memory_lifecycle" "2.3" \
$CC $CFLAGS \
test/unit/test_secure_memory_unit.c \
$UNITY_SRC \
src/security/secure_memory.c \
-o test_secure_memory_unit.exe && \
./test_secure_memory_unit.exe


run_test "device_identity_authentication" "2.4" \
$CC $CFLAGS \
test/unit/test_device_identity_unit.c \
$UNITY_SRC \
src/security/device_identity.c \
src/crypto/sha256.c \
-o test_device_identity_unit.exe && \
./test_device_identity_unit.exe


run_test "srp_zero_knowledge_authentication" "2.5" \
$CC $CFLAGS \
test/unit/test_srp_auth_unit.c \
$UNITY_SRC \
src/security/srp_auth.c \
src/crypto/sha256.c \
-o test_srp_auth_unit.exe && \
./test_srp_auth_unit.exe



echo "============================================================"
echo " ALL UNIT TESTS PASSED"
echo " TOTAL TEST MODULES EXECUTED: $TOTAL_TESTS"
echo "============================================================"