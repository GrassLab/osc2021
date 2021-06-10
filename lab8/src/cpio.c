# include "cpio.h"
# include "my_math.h"
# include "my_string.h"
# include "uart.h"
# include "log.h"

inline unsigned long align4(unsigned long n){
  return ((n + 3) & ~0x3);
}

void cpio_list(){
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
    LOG(FINE) uart_puts("\t");
    else uart_puts("\n");
    
    char print_c[20];
    int_to_hex(mode, print_c);
    log_puts(print_c, FINE);
    log_puts("  ", FINE);
    int_to_str(filesize, print_c);
    log_puts(print_c, FINE);
    log_puts("  ", FINE);
    int_to_str(namesize, print_c);
    log_puts(print_c, FINE);
    log_puts("\n", FINE);
    
    char *context = (char *) align4((unsigned long)name+namesize);
    
    blk = (cpio_newc_header *) align4((unsigned long)context+filesize);
    name = ((char *)blk + sizeof(cpio_newc_header));
  }
}

void cpio_show_file(char *file_name){

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

int cpio_get_argc(){
  int i = 0;
  cpio_newc_header *blk = INITRAMFS_ADDR;
  char *name = ((char *)blk + sizeof(cpio_newc_header));
  while(str_cmp(name, (char *) "TRAILER!!!") != 1){
    unsigned long filesize = hex_to_uint(blk->c_filesize, 8);
    unsigned long namesize = hex_to_uint(blk->c_namesize, 8);
    i++;
    char *context = (char *) align4((unsigned long)name+namesize);
    blk = (cpio_newc_header *) align4((unsigned long)context+filesize);
    name = ((char *)blk + sizeof(cpio_newc_header));
  }
  return i;
}

void cpio_get_argv(char **argv){
  int i = 0;
  cpio_newc_header *blk = INITRAMFS_ADDR;
  char *name = ((char *)blk + sizeof(cpio_newc_header));
  while(str_cmp(name, (char *) "TRAILER!!!") != 1){
    unsigned long filesize = hex_to_uint(blk->c_filesize, 8);
    unsigned long namesize = hex_to_uint(blk->c_namesize, 8);
    argv[i] = name;
    i++;
    char *context = (char *) align4((unsigned long)name+namesize);
    blk = (cpio_newc_header *) align4((unsigned long)context+filesize);
    name = ((char *)blk + sizeof(cpio_newc_header));
  }
}

int cpio_get_content(char *file_name, char **content){
  cpio_newc_header *blk = INITRAMFS_ADDR;
  char *name = ((char *)blk + sizeof(cpio_newc_header));
  while(str_cmp(name, (char *) "TRAILER!!!") != 1){
    unsigned long mode = hex_to_uint(blk->c_mode, 8);
    unsigned long filesize = hex_to_uint(blk->c_filesize, 8);
    unsigned long namesize = hex_to_uint(blk->c_namesize, 8);
    char *context = (char *) align4((unsigned long)name+namesize);
    if (str_cmp(name, file_name) == 1){
      if (mode & (1 << 14)){
        return 0;
      }
      else{
        *content = context;
        return filesize;
      }
    }
    blk = (cpio_newc_header *) align4((unsigned long)context+filesize);
    name = ((char *)blk + sizeof(cpio_newc_header)); 
  }
  return -1;
}
