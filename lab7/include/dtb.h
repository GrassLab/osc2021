#ifndef __DTB_H_
#define __DTB_H_
#include <stdint.h>
#include <stddef.h>
#include <list.h>

typedef struct dtb_prop {
  char *name;
  uint32_t size;
  void *data;
  struct list_head sibling;
} dtb_prop;

typedef struct dtb_node {
  char *name;
  struct list_head prop_list;
  struct list_head child;
  struct list_head sibling;
} dtb_node;

dtb_node *build_device_tree(void *_dtb);
dtb_prop *find_device_tree(dtb_node *node, const char *path);
void print_device_tree(dtb_node *dtb);

#endif