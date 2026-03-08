#ifndef SRP_AUTH_H
#define SRP_AUTH_H

#include <stdint.h>
#include <stddef.h>

#define SRP_KEY_SIZE 32
#define SRP_NONCE_SIZE 32
#define SRP_PROOF_SIZE 32

typedef struct {
    uint8_t salt[16];
    uint8_t verifier[SRP_KEY_SIZE];
} SrpVerifier;

typedef struct {
    uint8_t A[SRP_KEY_SIZE];
} SrpClientHello;

typedef struct {
    uint8_t salt[16];
    uint8_t B[SRP_KEY_SIZE];
} SrpServerChallenge;

typedef struct {
    uint8_t proof[SRP_PROOF_SIZE];
} SrpClientProof;

typedef struct {
    uint8_t proof[SRP_PROOF_SIZE];
} SrpServerProof;

/* registration */
int srp_create_verifier(
    const uint8_t *password,
    size_t len,
    SrpVerifier *out);

/* client start */
int srp_client_begin(
    SrpClientHello *hello,
    uint8_t client_secret[SRP_KEY_SIZE]);

/* server challenge */
int srp_server_challenge(
    const SrpVerifier *verifier,
    SrpServerChallenge *challenge,
    uint8_t server_secret[SRP_KEY_SIZE]);

/* client proof */
int srp_client_proof(
    const uint8_t *password,
    size_t len,
    const SrpServerChallenge *challenge,
    const SrpClientHello *hello,
    const uint8_t client_secret[SRP_KEY_SIZE],
    SrpClientProof *proof);

/* server verification */
int srp_server_verify(
    const SrpVerifier *verifier,
    const SrpClientHello *hello,
    const SrpServerChallenge *challenge,
    const uint8_t server_secret[SRP_KEY_SIZE],
    const SrpClientProof *proof,
    SrpServerProof *response);

#endif