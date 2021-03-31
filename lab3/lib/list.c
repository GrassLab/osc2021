#include "include/uart.h"
#include "include/list.h"

void list_init(struct list_head *node) {
    node->next = node;
    node->prev = node;
}

void insert_head(struct list_head *head, struct list_head *v) {
    v->next = head->next;
    v->prev = head;
    head->next->prev = v;
    head->next = v;
}

void insert_tail(struct list_head *head, struct list_head *v) {
    v->next = head;
    v->prev = head->prev;
    head->prev->next = v;
    head->prev = v;
}

struct list_head *remove_head(struct list_head *head) {
    struct list_head *ptr;
    ptr = head->next;
    head->next = head->next->next;
    head->next->prev = head;

    return ptr;
}

struct list_head *remove_tail(struct list_head *head) {
    struct list_head *ptr;
    ptr = head->prev;
    head->prev = head->prev->prev;
    head->prev->next = head;

    return ptr;
}

void unlink(struct list_head *node) {
    struct list_head *next, *prev;
    next = node->next;
    prev = node->prev;
    
    next->prev = prev;
    prev->next = next;
}