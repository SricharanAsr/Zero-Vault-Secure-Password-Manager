#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "security/srp_auth.h"
#include "crypto/sha256.h"

static void random_bytes(uint8_t *buf,size_t len)
{
    for(size_t i=0;i<len;i++)
        buf[i]=rand() & 0xFF;
}

static void hash2(
    const uint8_t *a,size_t alen,
    const uint8_t *b,size_t blen,
    uint8_t out[SRP_KEY_SIZE])
{
    Sha256Ctx ctx;

    sha256_init(&ctx);
    sha256_update(&ctx,a,alen);
    sha256_update(&ctx,b,blen);
    sha256_final(&ctx,out);
}

int srp_create_verifier(
    const uint8_t *password,
    size_t len,
    SrpVerifier *out)
{
    if(!password||!out)
        return -1;

    srand((unsigned)time(NULL));

    random_bytes(out->salt,16);

    uint8_t x[SRP_KEY_SIZE];

    hash2(password,len,out->salt,16,x);

    memcpy(out->verifier,x,SRP_KEY_SIZE);

    return 0;
}

int srp_client_begin(
    SrpClientHello *hello,
    uint8_t client_secret[SRP_KEY_SIZE])
{
    random_bytes(client_secret,SRP_KEY_SIZE);

    memcpy(hello->A,client_secret,SRP_KEY_SIZE);

    return 0;
}

int srp_server_challenge(
    const SrpVerifier *verifier,
    SrpServerChallenge *challenge,
    uint8_t server_secret[SRP_KEY_SIZE])
{
    random_bytes(server_secret,SRP_KEY_SIZE);

    memcpy(challenge->salt,verifier->salt,16);
    memcpy(challenge->B,server_secret,SRP_KEY_SIZE);

    return 0;
}

int srp_client_proof(
    const uint8_t *password,
    size_t len,
    const SrpServerChallenge *challenge,
    const SrpClientHello *hello,
    const uint8_t client_secret[SRP_KEY_SIZE],
    SrpClientProof *proof)
{
    uint8_t x[SRP_KEY_SIZE];

    hash2(password,len,challenge->salt,16,x);

    Sha256Ctx ctx;

    sha256_init(&ctx);

    sha256_update(&ctx,hello->A,SRP_KEY_SIZE);
    sha256_update(&ctx,challenge->B,SRP_KEY_SIZE);
    sha256_update(&ctx,x,SRP_KEY_SIZE);

    sha256_final(&ctx,proof->proof);

    return 0;
}

int srp_server_verify(
    const SrpVerifier *verifier,
    const SrpClientHello *hello,
    const SrpServerChallenge *challenge,
    const uint8_t server_secret[SRP_KEY_SIZE],
    const SrpClientProof *proof,
    SrpServerProof *response)
{
    uint8_t expected[SRP_PROOF_SIZE];

    Sha256Ctx ctx;

    sha256_init(&ctx);

    sha256_update(&ctx,hello->A,SRP_KEY_SIZE);
    sha256_update(&ctx,challenge->B,SRP_KEY_SIZE);
    sha256_update(&ctx,verifier->verifier,SRP_KEY_SIZE);

    sha256_final(&ctx,expected);

    if(memcmp(expected,proof->proof,SRP_PROOF_SIZE)!=0)
        return -1;

    sha256_init(&ctx);

    sha256_update(&ctx,proof->proof,SRP_PROOF_SIZE);
    sha256_update(&ctx,server_secret,SRP_KEY_SIZE);

    sha256_final(&ctx,response->proof);

    return 0;
}