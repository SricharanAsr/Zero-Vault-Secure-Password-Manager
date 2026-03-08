#ifndef SECURE_MEMORY_H
#define SECURE_MEMORY_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t length;
    int active;
} SecureBuffer;

/* allocate secure buffer */
int secure_alloc(SecureBuffer *buf, size_t len);

/* zeroize and free buffer */
void secure_free(SecureBuffer *buf);

/* securely wipe memory */
void secure_zero(void *ptr, size_t len);

#endif