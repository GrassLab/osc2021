#ifndef _LIST_H_
#define  _LIST_H_

#include "type.h"

typedef struct list_head {
    struct list_head *prev;
    struct list_head *next;
} list_head_t;

#define list_entry(ptr, type, member) \
((type*)((char*)(ptr) - (unsigned long)(&(((type*)0)->member))))

void list_init(list_head_t*);
void list_add(list_head_t*,
              list_head_t*);
void list_add_tail(list_head_t*,
                   list_head_t*);
void list_del(list_head_t*);
bool_t list_empty(list_head_t*);

#endif
