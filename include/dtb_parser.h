#ifndef __DTB_PARSER_H__
#define __DTB_PARSER_H__
#include "uart.h"
#include "system.h"
#include "string.h"
#include "utils.h"

#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009



void dtb_init(char* args);
void parse_dtb(char*, void*);
int __check_dtb(struct fdt_header*);
void extract_fdt_header(struct fdt_header*);
void unflatten_fdt(char** addr, struct fdt_header* header,char* args, int depth, void(*callback)(char*, struct fdt_header*, int));
void parse_node_property(char** addr, struct fdt_header* header, int depth);
void __print_string_align(char** addr);
void __print_alignchar(char c, int depth);
short __check_compatible(char**, struct fdt_header*, char*);
void __show_device_node_info(char* node_addr, struct fdt_header* header, int depth);
#endif