#ifndef _QUEUE_H_
#define  _QUEUE_H_

#include "type.h"

typedef struct {
    unsigned int front;
    unsigned int back;
    unsigned int size;
    bool_t       lock;
    void *buffer;
} queue_t;

void queue_init(queue_t*, void*, unsigned int);
int queue_push(queue_t*);
int queue_pop(queue_t*);
bool_t queue_empty(queue_t*);
bool_t queue_full(queue_t*);

static inline void queue_lock(queue_t *queue) {
    queue->lock = true;
}

static inline void queue_unlock(queue_t *queue) {
    queue->lock = false;
}

#endif
