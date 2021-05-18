#include "list.h"

void list_init(list_head_t *entry) {
    entry->next = entry;
    entry->prev = entry;
}

static void _list_add(list_head_t *prev,
                      list_head_t *next,
                      list_head_t *new) {
    new->next  = next;
    new->prev  = prev;
    prev->next = new;
    next->prev = new;
}

void list_add(list_head_t *head,
              list_head_t *new) {
    _list_add(head, head->next, new);
}

void list_add_tail(list_head_t *head,
                   list_head_t *new) {
    _list_add(head->prev, head, new);
}

void list_del(list_head_t *entry) {
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
    list_init(entry);
}

bool_t list_empty(list_head_t *head) {
    return head->next == head ? true : false;
}
