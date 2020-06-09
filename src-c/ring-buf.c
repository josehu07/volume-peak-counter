#include <stdlib.h>
#include <stdbool.h>

#include "ring-buf.h"


/**
 * This ring buffer implementation is not concurrently safe!
 */


RingBuf *
ring_buf_new(int size) {
    if (size > 0) {
        RingBuf *buf = malloc(sizeof(RingBuf));
        buf->size = size;   // Can contain size - 1 elements.
        buf->head = 0;
        buf->tail = 0;  // Head == tail means empty.
        buf->data = malloc(size * sizeof(float));
        return buf;
    } else {
        return NULL;
    }
}

void
ring_buf_del(RingBuf *buf) {
    if (buf != NULL) {
        if (buf->data != NULL)
            free(buf->data);
        free(buf);
    }
}

/** Overwrites the oldest element if full. */
void
ring_buf_push(RingBuf *buf, float val) {
    if (buf != NULL) {
        int new_tail = (buf->tail + 1) % buf->size;
        if (new_tail == buf->head)  // Full, so discard oldest element.
            buf->head = (buf->head + 1) % buf->size;
        buf->data[buf->tail] = val;
        buf->tail = new_tail;
    }
}
