#ifndef __WAITQUEUE_H_
#define __WAITQUEUE_H_
#include <stddef.h>

struct waitqueue *alloc_waitqueue();
void wait(struct waitqueue *wq) ;
void wakeup(struct waitqueue *wq);

#endif