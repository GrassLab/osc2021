#include "shell.h"

void shell() {
  uart_puts("*****************************Hello World*****************************\r\n");
  uart_puts("% ");
  char command[256];
  memset(command, '\0');
  int i = 0;
  while(1) {
    char c = uart_getc();
    c = c=='\r'?'\n':c;
    if(c != '\n') {
      command[i++] = c;
      uart_send(c);
    }
    else {
      command[i] = '\0';
      uart_puts("\n");
      if(i > 0) 
        do_command(command);
      
      uart_puts("% ");
      i = 0;
      memset(command, '\0');
    }
  }
}
void do_command(char* command) {
  if(strncmp(command, "help", 5) == 0) {
        uart_puts("help: print all available commands.\n");
        uart_puts("hello: print Hello World!.\n");
        uart_puts("reboot: reboot rpi3.\n");
        uart_puts("loadimg: load kernel image.\n");
        uart_puts("ls: list cpio files.\n");
        uart_puts("cat [file]: cat cpio file.\n");
  }
  else if(strncmp(command, "hello", 6) == 0) {
    uart_puts("Hello World!\n");
  }
  else if(strncmp(command, "reboot", 7) == 0) {
    reset(100);
  }
  else if(strncmp(command, "loadimg", 7) == 0) {
    loadimg();
  }
  else if(strncmp(command, "cat ", 4) == 0) {
    get_file_content(command + 4, strlen(command + 4));
  }
  else if(strncmp(command, "ls", 3) == 0) {
    get_all_pathname();
  }
  else if(strncmp(command, "lsbss", 6) == 0) {
    extern void *_bss_begin;
    extern void *_bss_end;
    uart_puts("bss_begin: \n");
    uart_hex((unsigned long)&_bss_begin);
    uart_puts("\n_bss_end\n");
    uart_hex((unsigned long)&_bss_end);
  }
  else {
    uart_puts("unknown command\n");
  }	
}

void loadimg() {
  size_t load_address;
  char buf[9]; 
  size_t img_size;
  //count bootloader size
  extern void *_start_bootloader;
  extern void *_end_bootloader;
  size_t _bootloader_size = (size_t) &_end_bootloader - (size_t) &_start_bootloader;
  //read load address
  uart_puts("Input the address to load image(0x): ");
  uart_readline(buf, 8);
  load_address = strtol(buf, 0, 16);
  uart_puts("\nLoad image at: 0x");
  uart_hex(load_address);
  //read size
  uart_puts("\nInput the image size(0x): ");
  uart_read(buf, 8);
  buf[8] = '\0';
  img_size = strtol(buf, 0, 16);
  uart_puts("\nimage size: 0x");
  uart_hex(img_size);
  uart_puts("\n");
  //check bootloader, and image is overlap 
  size_t img_end = load_address + img_size;
  size_t relocated_readimg_jump = (size_t)&readimg_jump;
  if(img_end > (size_t) &_start_bootloader) {
    uart_puts("image overlapped to bootloader.\n");
    size_t relocated_bootloader = img_end + BOOTLOADER_OFFSET;
    //relocate bootloader
    memcpy((char *)relocated_bootloader, &_start_bootloader, _bootloader_size);
    //jump to rest of code 
    relocated_readimg_jump = relocated_bootloader + ((size_t)&readimg_jump - (size_t)&_start_bootloader);
    //relocated readimg_jump address
    uart_puts("relocated rest of bootloader to address: ");
    uart_hex(relocated_readimg_jump);
    uart_puts("\n");
  }
  //jump to readimg_jump
  asm volatile ("mov x0, %0\n" "mov x1, %1\n" "mov sp, %2\n" "blr %3\n"::
  "r" (load_address),
  "r" (img_size),
  "r" (load_address),
	"r" (relocated_readimg_jump):"x0", "x1");
}
//read kernel img, and jump 
void readimg_jump(size_t load_address, size_t img_size) {
  uart_read((char* )load_address, img_size);
  ((void (*)(void))(load_address))();
}

