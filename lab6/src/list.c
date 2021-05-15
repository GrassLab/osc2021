# include "list.h"
# include "log.h"

void list_head_init(struct list_head *l) {
    l->next = l;
    l->prev = l;
}

int list_is_empty(struct list_head *l) {
    return l->next == l;
}

static void __list_add(struct list_head *new_lst,
                       struct list_head *prev,
                       struct list_head *next) {
    next->prev = new_lst;
    new_lst->next = next;
    new_lst->prev = prev;
    prev->next = new_lst;
}

// add new_lst to head->next
void list_add_next(struct list_head *new_lst, struct list_head *head) {
    __list_add(new_lst, head, head->next);
}

// add new_lst to head->prev
void list_add_prev(struct list_head *new_lst, struct list_head *head) {
    __list_add(new_lst, head->prev, head);
}

static void __list_del(struct list_head *prev,
                       struct list_head *next) {
    next->prev = prev;
    prev->next = next;
}

void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    entry->next = entry;
    entry->prev = entry;
}
