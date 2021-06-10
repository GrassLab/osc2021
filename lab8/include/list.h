#ifndef __LIST_H_
#define __LIST_H_
#include <stdint.h>
#include <stddef.h>

#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

#define container_of(ptr, type, member) ({ \
    void *__mptr = (void *)(ptr);          \
    ((type *)(__mptr - offsetof(type, member))); })

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define LIST_HEAD_INIT(name) (struct list_head){ &(name), &(name) }

struct list_head {
    struct list_head *next, *prev;
};

static inline int list_empty(const struct list_head *head){
	return head->next == head;
}

void list_init(struct list_head *node);
void insert_head(struct list_head *head, struct list_head *v);
void insert_tail(struct list_head *head, struct list_head *v);
struct list_head *remove_head(struct list_head *head);
struct list_head *remove_tail(struct list_head *head);
void unlink(struct list_head *node);

#endif