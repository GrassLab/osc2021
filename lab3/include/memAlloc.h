#ifndef _MEMALLOC_H
#define _MEMALLOC_H

typedef int bool;
#define true 1
#define false 0
#define NULL 0
typedef struct __list_node{

  struct __list_node *prev, *next;
  int start_addr;
  int buddy_index;
  int fr_no;
}list_node;

//typedef struct __list_head{
//
//  list_node *prev, *next;
//}list_head;

typedef struct __node_info{
  list_node* corespond_list_node;
  int inused;
  int buddy;
  int exp;
}node_info;

typedef struct __chunk_info{
  int onused;
}chunk_info;
void list_node_push(list_node* entry,int exp);
void list_node_pop(int exp);
void list_node_del(list_node* entry);
void list_remove(list_node *entry);
bool check_list(int index);
int mem_init();
void* my_alloc(int size);
void my_free(void *addr);
void* dy_alloc(int size);


#endif
