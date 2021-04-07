#include "list.h"

void init_list(list_head *l) {
  l->next = l;
  l->prev = l;
}

void push_list(list_head *l,list_head *chunk) {
  chunk->prev = l;
  chunk->next = l->next;
  l->next->prev = chunk;
  l->next = chunk;
}

void pop_list(list_head *chunk) {
  chunk->next->prev = chunk->prev;
  chunk->prev->next = chunk->next;
}

int list_empty(list_head *l) { 
	return l->next == l; 
}



