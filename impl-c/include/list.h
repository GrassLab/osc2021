#pragma once

#include "bool.h"
#include <stddef.h>

// Circular linked list

// list_head is a base struct for using linked list
//
// An object utilizing these functions must have a list_head struct at
//  the beginning of it's struct definition
// (This is how inheritance be done in C)
//
// An linked list could be seen as followed:
//  list_head -> entry -> entry -> ...
// The list_head is a place holder

typedef struct list_head {
  struct list_head *prev, *next;
} list_head_t;

// Push a node to the back of a list
static inline void list_push(struct list_head *entry, struct list_head *list) {
  entry->prev = list->prev;
  entry->next = list;
  list->prev->next = entry;
  list->prev = entry;
}

// Remove a node from list
// no need to specify the list becuase it's a Circular doubly linked list
static inline void list_del(struct list_head *entry) {
  struct list_head *prev = entry->prev;
  struct list_head *next = entry->next;
  prev->next = next;
  next->prev = prev;

  entry->next = NULL;
  entry->prev = NULL;
}

// Check if a list is empty
// !! Caution: must called with the list head, otherwise would produce wrong
// !! result
static inline bool list_empty(struct list_head *head) {
  return head->next == head;
}

// Initailize a list
// !! Caution: must called with the list head, otherwise would produce wrong
// !! result
static inline void list_init(struct list_head *head) {
  head->next = head;
  head->prev = head;
}

// Pop an entry from list, assuming that the list is not empty
// !! Caution: must called with the list head, otherwise would produce wrong
// !! result
static inline struct list_head *list_pop(struct list_head *list) {
  struct list_head *entry = list->prev;
  list_del(entry);
  return entry;
}
