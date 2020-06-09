#ifndef RING_BUF_H
#define RING_BUF_H


typedef struct {
    int size;
    int head;
    int tail;
    float *data;
} RingBuf;


RingBuf *ring_buf_new(int size);
void ring_buf_del(RingBuf *buf);
void ring_buf_push(RingBuf *buf, float val);


/**
 * Use the following pattern to iteratively read through a RingBuf:
 *   for (int view = ring_buf_view(buf);
 *        view != -1;
 *        view = ring_buf_next(buf, view)) {
 *        // Do something...
 *   }
 */
inline int
ring_buf_view(RingBuf *buf) {
    if (buf != NULL && buf->head != buf->tail)
        return buf->head;
    return -1;
}

inline int
ring_buf_next(RingBuf *buf, int view) {
    if (buf != NULL && buf->head != buf->tail) {
        int next_view = (view + 1) % buf->size;
        if (next_view != buf->tail)
            return next_view;
    }
    return -1;
}

/** Assumes idx is valid, else returns 0.0. */
inline float
ring_buf_read(RingBuf *buf, int idx) {
    if (buf != NULL && idx >= 0 && idx < buf->size)
        return buf->data[idx];
    return 0.0;
}


#endif // RING_BUF_H
