#include "devicetree.h"

void devicetree_parse(void* address, int mode, char* dev_name) {
  /*uart_puts("load_address: \n");
  uart_hex(address);
  uart_puts("\n");*/
  dtb_address = address;
  //magic number
  //uint32_t magic = bytes_to_uint32_t(address);
  /*uart_puts("magic: \n");
  uart_hex(magic);
  uart_puts("\n");*/
  //offset of structure block
  uint32_t off_dt_struct = bytes_to_uint32_t(address + DEVICETREE_OFF_DT_STRUCT_OFFSET);
  /*uart_puts("off_dt_struct: \n");
  uart_hex(off_dt_struct);
  uart_puts("\n");*/
  //offset of strings block
  uint32_t off_dt_strings = bytes_to_uint32_t(address + DEVICETREE_OFF_DT_STRINGS_OFFSET);
  /*uart_puts("off_dt_strings: \n");
  uart_hex(off_dt_strings);
  uart_puts("\n");*/

  address += off_dt_struct;
 
  uint32_t property_len, property_name_offset;
  void* name_address = 0;
  //parse structure block
  while(1) {
    uint32_t token = bytes_to_uint32_t(address);
    address += 4;
    
    switch(token) {
      case FDT_BEGIN_NODE:
        //get unit name
        name_address = address;
        if(mode == DISPLAY_DEVICE_NAME) {
          /*uart_puts("name: ");
          uart_puts((char *)address);
          uart_puts("\n");*/
        }
        address += strlen((char *)address);
        /* padding */
         if((size_t)address % 4 != 0)
           address += 4 - ((size_t)address % 4);
        break;
      case FDT_END_NODE:
        //uart_puts("Node end\n");
        //node end, do nothing
        break;
      case FDT_PROP:
        //get property value length
        property_len = bytes_to_uint32_t(address);
        address += 4;
        //get perperty name offset in strings block
        property_name_offset = bytes_to_uint32_t(address);
        address += 4;
        if(mode == DISPLAY_DEVICE_PROPERTY && strlen(dev_name) > 0 && strncmp((char *)name_address, dev_name, strlen(dev_name)) == 0 && property_len > 0) {
          uart_puts("property name: ");
          uart_puts((char *)(dtb_address + off_dt_strings + property_name_offset));
          uart_puts("\n");
          //property value
          uart_puts("----");
          do_uart_write((char *)address, property_len);
          uart_puts("\n");
        }
        //strncpy(, (char *)address, property_len);
        address += property_len;
        /* padding */
        if((size_t)address % 4 != 0)
          address += 4 - ((size_t)address % 4);
        break;
      case FDT_NOP:
        //do nothing
        break;
      case FDT_END:
        //structure block end
        return;
      default:
        break;
    }
  }
}

uint32_t bytes_to_uint32_t(void* address) {
  uint32_t n = *(uint8_t *)(address) << 24 |
    *(uint8_t *)(address + 1) << 16 |
    *(uint8_t *)(address + 2) << 8 |
    *(uint8_t *)(address + 3);
  return n;
}

void* get_dtb_address() {
    return dtb_address;
}

void set_dtb_address(void* address) {
  dtb_address = address;
}