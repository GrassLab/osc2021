#include "queue.h"

void queue_init(queue_t *queue, void *buffer, unsigned int size) {
    queue->front = 0;
    queue->back = 0;
    queue->size = size;
    queue->buffer = buffer;
}

int queue_push(queue_t *queue) {
    if (queue_full(queue))
        return -1;
    return queue->back = (queue->back + 1) % queue->size;
}

int queue_pop(queue_t *queue) {
    if (queue_empty(queue))
        return -1;
    return queue->front = (queue->front + 1) % queue->size;
}

bool_t queue_empty(queue_t *queue) {
    return queue->front == queue->back ? true : false;
}

bool_t queue_full(queue_t *queue) {
    return queue->front == ((queue->back + 1) % queue->size) ? true : false;
}
