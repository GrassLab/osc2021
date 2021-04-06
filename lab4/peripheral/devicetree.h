#ifndef DEVICETREE_H
#define DEVICETREE_H
#include <types.h>
#include <uart.h>
#include <string.h>
size_t dtb_begin;

/**
 * struct fdt_header {
 *   uint32_t magic;
 *   uint32_t totalsize;
 *   uint32_t off_dt_struct;
 *   uint32_t off_dt_strings;
 *   uint32_t off_mem_rsvmap; 
 *   uint32_t version;
 *   uint32_t last_comp_version;
 *   uint32_t boot_cpuid_phys;
 *   uint32_t size_dt_strings;
 *   uint32_t size_dt_struct;
 * };
 */
/* header element offset */
#define DEVICETREE_TOTALSIZE_OFFSET 4
#define DEVICETREE_OFF_DT_STRUCT_OFFSET 8
#define DEVICETREE_OFF_DT_STRINGS_OFFSET 12
#define DEVICETREE_SIZE_DT_STRINGS_OFFSET 32
#define DEVICETREE_SIZE_DT_STRUCT_OFFSET 36
/* structure block token */
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
/**
 * struct {
 *   uint32_t len;  //length of property value of bytes
 *   uint32_t nameoff; //property name offset
 * }
 * followed by perperty value.
 */
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

#define DISPLAY_DEVICE_NAME 1
#define DISPLAY_DEVICE_PROPERTY 2
size_t dtb_address;

void devicetree_parse(size_t address, int mode, char* dev_name);
uint32_t bytes_to_uint32_t(size_t address);
size_t get_dtb_address();
void set_dtb_address(size_t address);
#endif