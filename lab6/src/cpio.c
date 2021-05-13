# include "cpio.h"
# include "load_app.h"
# include "my_math.h"
# include "my_string.h"
# include "uart.h"

inline unsigned long align4(unsigned long n){
  return ((n + 3) & ~0x3);
  //if (n%4) return ((n >> 2) + 1 ) << 2;
  //else return n;
}

void list(){
uart_puts((char *) "List of files:\n");

  cpio_newc_header *blk = INITRAMFS_ADDR;

  char *name = ((char *)blk + sizeof(cpio_newc_header));
  while(str_cmp(name, (char *) "TRAILER!!!") != 1){
    unsigned long mode = hex_to_uint(blk->c_mode, 8);
    unsigned long filesize = hex_to_uint(blk->c_filesize, 8);
    unsigned long namesize = hex_to_uint(blk->c_namesize, 8);
    if (mode & (1 << 14)) uart_puts((char *) " <DIR> ");
    else uart_puts((char *) "<FILE> ");

    uart_puts(name);
    uart_puts((char *) "\n");
    //int_to_hex(mode, print_c);
    //uart_puts(print_c);
    //uart_puts("\n");
    //int_to_str(filesize, print_c);
    //uart_puts(print_c);
    //uart_puts("\n");
    //int_to_str(namesize, print_c);
    //uart_puts(print_c);
    //uart_puts("\n");
    char *context = (char *) align4((unsigned long)name+namesize);
    /*
    int_to_hex(blk, print_c);
    uart_puts(print_c);
    uart_puts("\n");
    int_to_hex(align4(filesize), print_c);
    uart_puts(print_c);
    uart_puts("\n");
    int_to_hex(name, print_c);
    uart_puts(print_c);
    uart_puts("\n");
    int_to_hex(context, print_c);
    uart_puts(print_c);
    uart_puts("\n");
    */
    blk = (cpio_newc_header *) align4((unsigned long)context+filesize);
    name = ((char *)blk + sizeof(cpio_newc_header));
  }
}

void show_file(char *file_name){

  cpio_newc_header *blk = INITRAMFS_ADDR;

  char *name = ((char *)blk + sizeof(cpio_newc_header));
  while(str_cmp(name, (char *) "TRAILER!!!") != 1){
    unsigned long mode = hex_to_uint(blk->c_mode, 8);
    unsigned long filesize = hex_to_uint(blk->c_filesize, 8);
    unsigned long namesize = hex_to_uint(blk->c_namesize, 8);
    char *context = (char *) align4((unsigned long)name+namesize);
    if (str_cmp(name, file_name) == 1){
      if (mode & (1 << 14)){
        uart_puts((char *) "\"");
        uart_puts(file_name);
        uart_puts((char *) "\" is a directory.\n");
        return;
      }
      else{
        uart_puts_n(context, filesize);
        uart_puts((char *) "\n");
        return;
      }
    }
    blk = (cpio_newc_header *) align4((unsigned long)context+filesize);
    name = ((char *)blk + sizeof(cpio_newc_header)); 
  }
  uart_puts((char *) "File \"");
  uart_puts(file_name);
  uart_puts((char *) "\" not found.\n");

  return;
}

void exec_app(char *file_name){
  cpio_newc_header *blk = INITRAMFS_ADDR;

  char *name = ((char *)blk + sizeof(cpio_newc_header));
  while(str_cmp(name, (char *) "TRAILER!!!") != 1){
    unsigned long mode = hex_to_uint(blk->c_mode, 8);
    unsigned long filesize = hex_to_uint(blk->c_filesize, 8);
    unsigned long namesize = hex_to_uint(blk->c_namesize, 8);
    char *context = (char *) align4((unsigned long)name+namesize);
    if (str_cmp(name, file_name) == 1){
      if (mode & (1 << 14)){
        uart_puts((char *) "\"");
        uart_puts(file_name);
        uart_puts((char *) "\" is a directory.\n");
        return;
      }
      else{
        load_app((void *)context, filesize);
        return;
      }
    }
    blk = (cpio_newc_header *) align4((unsigned long)context+filesize);
    name = ((char *)blk + sizeof(cpio_newc_header)); 
  }
  uart_puts((char *) "File \"");
  uart_puts(file_name);
  uart_puts((char *) "\" not found.\n");
}
