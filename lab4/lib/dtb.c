#include "dtb.h"
#include "string.h"
#include "mm.h"
#include "list.h"
#include "uart.h"

#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

typedef struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
} fdt_header;

static uint32_t get_be_int(void *ptr) {
    unsigned char *bytes = ptr;
    uint32_t ret = bytes[3];
    ret |= bytes[2] << 8;
    ret |= bytes[1] << 16;
    ret |= bytes[0] << 24;

    return ret;
}

static uint32_t align_up(uint32_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

static dtb_node *build_node(uintptr_t *_ptr, uintptr_t strings, char *name) {
  uintptr_t ptr = *_ptr;
  dtb_node *node = kmalloc(sizeof(dtb_node));

  list_init(&node->child);
  list_init(&node->sibling);
  list_init(&node->prop_list);
  node->name = name;

  while (1) {
    uint32_t token = get_be_int((char *)ptr);
    ptr += 4;

    switch (token) {
      case FDT_BEGIN_NODE: {
        char *_name = (char *)ptr;
        ptr += align_up(strlen((char *)ptr) + 1, 4);

        dtb_node *child_node = build_node(&ptr, strings, _name);
        insert_tail(&node->child, &child_node->sibling);
        break;
      }

      case FDT_END_NODE:
        *_ptr = ptr;
        return node;

      case FDT_NOP:
        break;

      case FDT_PROP: {
        uint32_t size = get_be_int((char *)ptr);
        ptr += 4;
        uint32_t nameoff = get_be_int((char *)ptr);
        ptr += 4;

        dtb_prop *prop = kmalloc(sizeof(dtb_prop));
        prop->data = (void *)ptr;
        prop->size = size;
        prop->name = (char *)strings + nameoff;
        insert_tail(&node->prop_list, &prop->sibling);

        ptr += align_up(size, 4);
        break;
      }

      case FDT_END:
        break;

      default:
        return NULL;
    }
  }

  return NULL;
}

dtb_node *build_device_tree(void *_dtb) {
  uintptr_t dtb = (uintptr_t)_dtb;
  fdt_header *header = (fdt_header *)dtb;
  if (get_be_int(&header->magic) != 0xd00dfeed) {
    return NULL;
  }

  uintptr_t dt_struct_ptr = dtb + get_be_int(&header->off_dt_struct);
  uintptr_t dt_strings_ptr = dtb + get_be_int(&header->off_dt_strings);

  uint32_t token = get_be_int((char *)dt_struct_ptr);
  dt_struct_ptr += 4;

  if (token != FDT_BEGIN_NODE) {
    return NULL;
  }

  dt_struct_ptr += align_up(strlen((char *)dt_struct_ptr) + 1, 4);
  dtb_node *node = build_node(&dt_struct_ptr, dt_strings_ptr, "/");

  token = get_be_int((char *)dt_struct_ptr);
  if (token != FDT_END) {
    return NULL;
  }

  return node;
}

static void write_indent(int n) {
  while (n--) write_uart(" ", 1);
}

static void print_node(dtb_node *node, int depth) {
  write_indent(depth);
  print_uart(node->name);
  puts_uart(" {");
  struct list_head *pos;

  list_for_each(pos, &node->prop_list) {
    dtb_prop *now = list_entry(pos, dtb_prop, sibling);
    write_indent(depth + 1);
    puts_uart(now->name);
  }

  list_for_each(pos, &node->child) {
    dtb_node *now = list_entry(pos, dtb_node, sibling);
    print_node(now, depth + 1);
  }
  write_indent(depth);
  puts_uart("}");
}

void print_device_tree(dtb_node *node) {
  print_node(node, 0);
}

static dtb_prop *find_prop(dtb_node *node, const char *args[], int len) {
  struct list_head *pos;
  int i = 0;

  if (len < 1) {
    return NULL;
  }

  list_for_each(pos, &node->child) {
    dtb_node *now = list_entry(pos, dtb_node, sibling);
    if (!strcmp(now->name, args[i])) {
      node = now;
      pos = &node->child;
      i++;
    }
  }

  if (i != len - 1) {
    return NULL;
  }

  list_for_each(pos, &node->prop_list) {
    dtb_prop *now = list_entry(pos, dtb_prop, sibling);
    if (!strcmp(now->name, args[i])) {
      return now;
    }
  }

  return NULL;
}

dtb_prop *find_device_tree(dtb_node *node, const char *path) {
  const char *args[0x10];
  char *_path = strdup(path);
  int len = strlen(path);
  int size = 0;

  for(int i = 0; i < len && size < 0x10; i++) {
    if (_path[i] != '/') {
      args[size++] = &_path[i];
      while (_path[i] && _path[i] != '/') i++;
      _path[i] = '\0';
    }
  }
  dtb_prop *prop = find_prop(node, args, size);
  kfree(_path);

  return prop;
}