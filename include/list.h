
#ifndef LIST_H
#define LIST_H

typedef struct list_head
{
    struct list_head *prev, *next;
} list_head_t;

static inline void list_init_head(list_head_t *list)
{
    list->prev = list->next = list;
}

static inline void __list_add(list_head_t *new_, list_head_t *prev, list_head_t *next)
{
    next->prev = new_;
    new_->next = next;
    new_->prev = prev;
    prev->next = new_;
}

static inline void list_add_head(list_head_t *new_, list_head_t *head)
{
    __list_add(new_, head, head->next);
}

static inline void list_add_tail(list_head_t *new_, list_head_t *head)
{
    __list_add(new_, head->prev, head);
}

// concatenate list
static inline void list_cat(list_head_t *new_h, list_head_t *new_t, list_head_t *head)
{
    new_h->prev = head;
    new_t->next = head->next;
    head->next->prev = new_t;
    head->next = new_h;
}

static inline void list_cat_tail(list_head_t *new_h, list_head_t *new_t, list_head_t *head)
{
    new_h->prev = head->prev;
    head->prev->next = new_h;
    head->prev = new_t;
    new_t->next = head;
}

static inline void list_crop(list_head_t *list_h, list_head_t *list_t)
{
    list_h->prev->next = list_t->next;
    list_t->next->prev = list_h->prev;
}

static inline int list_empty(const list_head_t *head)
{
    return head->next == head;
}

#endif