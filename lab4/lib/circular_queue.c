#include "circular_queue.h"
#include <printf.h>

int circular_queue_is_empty(struct circular_queue *q) {
  return q->front == q->rear && q->flag == 0;
}

int circular_queue_is_full(struct circular_queue *q) {
  return q->front == q->rear && q->flag == 1;
}

void circular_queue_push(struct circular_queue *q, char c) {
  if(circular_queue_is_full(q))
    return;
  q->buffer[q->rear] = c;
  /* char ch[2];
  ch[0] = c;
  ch[1] = '\0';
  printf("push: %s\n", ch);*/
  q->rear = (q->rear + 1) % CIRCULAR_QUEUE_BUFFER_SIZE;
  if(q->rear == q->front && q->flag == 0)
    q->flag = 1;
}

char circular_queue_pop(struct circular_queue *q) {
  if(circular_queue_is_empty(q))
    return -1;
  if(q->front == q->rear && q->flag == 1)
    q->flag = 0;
  char c;
  c = q->buffer[q->front];
  q->front = (q->front + 1) % CIRCULAR_QUEUE_BUFFER_SIZE; 
  return c;
}

void circular_queue_status(struct circular_queue *q) {
  printf("front: %d\n", q->front);
  printf("rear: %d\n", q->rear);
  //int i;
  /*i  = q->front;
  while(i % CIRCULAR_QUEUE_BUFFER_SIZE != q->rear) {
    printf("%d: %c\n", i, q->buffer[i]);
    i++;
  }*/
}