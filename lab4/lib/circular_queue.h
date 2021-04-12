#ifndef _CIRCULAR_QUEUE_H_
#define _CIRCULAR_QUEUE_H_

#define CIRCULAR_QUEUE_BUFFER_SIZE 0x200
struct circular_queue {
  int front;
  int rear;
  char buffer[CIRCULAR_QUEUE_BUFFER_SIZE];
  int flag;
};

int circular_queue_is_empty(struct circular_queue *q);
int circular_queue_is_full(struct circular_queue *q);
void circular_queue_push(struct circular_queue *q, char c);
char circular_queue_pop(struct circular_queue *q);
void circular_queue_status(struct circular_queue *q);

#endif