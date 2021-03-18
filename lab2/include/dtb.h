#ifndef __DTB_H_
#define __DTB_H_
#include <stdint.h>
#include <stddef.h>

#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

typedef void (*dtb_callback)(int type, const char *name, const void *data, uint32_t size);
int traverse_device_tree(void *dtb, dtb_callback cb);

#endif