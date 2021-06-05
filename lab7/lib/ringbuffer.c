#include <stddef.h>
#include <preempt.h>
#include <string.h>
#include <mm.h>
#include <atomic.h>

struct ring_buffer {
    char *head;
    char *tail;
    char *start;
    char *end;
    int size;
};

struct ring_buffer *alloc_ring_buffer(int size) {
    struct ring_buffer *rb = kmalloc(sizeof(struct ring_buffer));

    /* add one to avoid full case, also make it can be read/write simultaneously */
    rb->head = rb->tail = rb->start = kmalloc(size + 1);
    rb->size = size;
    rb->end = rb->start + size + 1;

    return rb;
}

/* it should be no need to mask interrupt */
int read_buffer(struct ring_buffer *rb, int size, char *outbuf) {
    disable_preempt();

    if (size < 0) {
        return -1;
    }

    char *head = rb->head;
    char *tail = rb->tail; /* producer may modify tail */
    int ret;

    /* implementation for optimized memcpy */
    if (head > tail) {
        int bsize = rb->end - head;
        int fsize = tail - rb->start;

        if (bsize >= size) {
            memcpy(outbuf, head, size);
            rb->head += size;
            ret = size;
        } else {
            memcpy(outbuf, head, bsize);
            if (fsize >= size - bsize) {
                memcpy(outbuf + bsize, rb->start, size - bsize);
                rb->head = rb->start + (size - bsize);
                ret = size;
            } else {
                memcpy(outbuf + bsize, rb->start, fsize);
                rb->head = rb->start + fsize;
                ret = bsize + fsize;
            }
        }
    } else {
        int avail = tail - head;

        if (size <= avail) {
            memcpy(outbuf, head, size);
            rb->head += size;
            ret = size;
        } else {
            memcpy(outbuf, head, avail);
            rb->head += avail;
            ret = avail;
        }
    }

    enable_preempt();
    return ret;
}

/* assuming there's only one producer */
/* TODO: add memory write barrier */
int write_buffer(struct ring_buffer *rb, int size, char *inbuf) {
    if (size < 0) {
        return -1;
    }

    int ret;

    if (rb->head > rb->tail) {
        int avail = rb->head - rb->tail - 1;
        int wsize = avail > size ? size : avail;

        memcpy(rb->tail, inbuf, wsize);
        rb->tail += wsize;
        ret = wsize;
    } else {
        int bsize = rb->end - rb->tail;
        int fsize = rb->head - rb->start;

        if (fsize == 0) {
            bsize -= 1;
        } else {
            fsize -= 1;
        }

        if (bsize >= size) {
            memcpy(rb->tail, inbuf, size);
            rb->tail += size;
            ret = size;
        } else {
            memcpy(rb->tail, inbuf, bsize);
            if (fsize >= size - bsize) {
                memcpy(rb->start, inbuf + bsize, size - bsize);
                rb->tail = rb->start + size - bsize;
                ret = size;
            } else {
                memcpy(rb->start, inbuf + bsize, fsize);
                rb->tail = rb->start + fsize;
                ret = bsize + fsize;
            }
        }
    }

    return ret;
}