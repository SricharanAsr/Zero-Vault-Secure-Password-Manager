#include <stdlib.h>
#include <string.h>

#include "security/secure_memory.h"

/* volatile pointer prevents compiler optimization */
static void secure_memwipe(void *ptr, size_t len)
{
    volatile uint8_t *p = ptr;

    while (len--)
        *p++ = 0;
}

int secure_alloc(SecureBuffer *buf, size_t len)
{
    if (!buf || len == 0)
        return -1;

    buf->data = malloc(len);

    if (!buf->data)
        return -1;

    buf->length = len;
    buf->active = 1;

    memset(buf->data, 0, len);

    return 0;
}

void secure_zero(void *ptr, size_t len)
{
    if (!ptr)
        return;

    secure_memwipe(ptr, len);
}

void secure_free(SecureBuffer *buf)
{
    if (!buf || !buf->data)
        return;

    secure_memwipe(buf->data, buf->length);

    free(buf->data);

    buf->data = NULL;
    buf->length = 0;
    buf->active = 0;
}