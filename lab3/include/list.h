#ifndef __LIST_H_
#define __LIST_H_

struct list_head {
    struct list_head *next, *prev;
};

struct list {
    struct list *next;
};

void insert_head(struct list_head *head, struct list_head *v);
void insert_tail(struct list_head *head, struct list_head *v);
struct list_head *remove_head(struct list_head *head);
struct list_head *remove_tail(struct list_head *head);
void unlink(struct list_head *node);

#endif