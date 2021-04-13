#include "shell.h"
#include <uart.h>
#include <reset.h>
#include <string.h>
#include <cpio.h>
#include <devicetree.h>
#include <buddy.h>
#include <dynamic.h>
#include <exception.h>
#include <printf.h>
#include <timer.h>

void shell() {
  uart_puts("*****************************Hello World*****************************\r\n");
  uart_puts("% ");
  char command[SHELL_COMMNAD_SIZE];
  memset(command, strlen(command), '\0');
  int i = 0;
  while(1) {
    char c = uart_getc();
    c = c=='\r'?'\n':c;
    if(c != '\n' && c != '\x7f') {
      command[i++] = c;
      uart_send(c);
    }
    else if(c == '\x7f') {
      uart_send('\x08');
      uart_send(' ');
      uart_send('\x08');
      command[--i] = '\0';
    }
    else {
      command[i] = '\0';
      uart_puts("\n");
      if(i > 0) 
        do_command(command);
      
      uart_puts("% ");
      i = 0;
      memset(command, strlen(command), '\0');
    }
  }
}
void do_command(char* command) {
  if(strncmp(command, "help", 5) == 0) {
    printf("help: print all available commands.\n");
    printf("hello: print Hello World!.\n");
    printf("reboot: reboot rpi3.\n");
    printf("loadimg: load kernel image.\n");
    printf("lscpio: list cpio files.\n");
    printf("lsdtb: list dtb node name.\n");
    printf("lsdtbprop [node name]: list [node name] property.\n");
    printf("cat [file]: cat cpio file.\n");
    printf("bmalloc [size]: buddy malloc.\n");
    printf("bfree [address]: buddy free.\n");
    printf("dmalloc [size]: dynamic malloc.\n");
    printf("dfree [address]: dynamic free.\n");
    printf("svc: trigger interrupt\n");
    printf("run: run user program in el0\n");
    printf("el12el0: from el1 to el0\n");
    printf("asyncw: asynchronous write\n");
    printf("asyncr: asynchronous read\n");
  }
  else if(strncmp(command, "hello", 6) == 0) {
    printf("Hello World!\n");
  }
  else if(strncmp(command, "reboot", 7) == 0) {
    reset(100);
  }
  else if(strncmp(command, "loadimg", 7) == 0) {
    loadimg();
  }
  else if(strncmp(command, "cat ", 4) == 0) {
    cpio_get_file_content(command + 4, strlen(command + 4));
  }
  else if(strncmp(command, "lscpio", 3) == 0) {
    cpio_get_all_pathname();
  }
  else if(strncmp(command, "lsbss", 6) == 0) {
    extern void *_bss_begin;
    extern void *_bss_end;
    printf("bss_begin: %x\n _bss_end %x\n", (unsigned long)&_bss_begin, (unsigned long)&_bss_end);
  }
  else if(strncmp(command, "lsdtb", 6) == 0) {
    devicetree_parse(get_dtb_address(), DISPLAY_DEVICE_NAME, null);
  }
  else if(strncmp(command, "lsdtbprop", 9) == 0) {
    devicetree_parse(get_dtb_address(), DISPLAY_DEVICE_PROPERTY, command + 10);
  }
  else if(strncmp(command, "bmalloc", 7) == 0) {
    buddy_malloc(strtol(command + 7, 0, 16));
  }
  else if(strncmp(command, "bfree", 5) == 0) {
    buddy_free((void *)strtol(command + 5, 0, 16));
  }
  else if(strncmp(command, "dmalloc", 7) == 0) {
    dynamic_malloc(strtol(command + 7, 0, 16));
  }
  else if(strncmp(command, "dfree", 5) == 0) {
    dynamic_free((void *)strtol(command + 5, 0, 16));
  }
  else if(strncmp(command, "svc", 4) == 0) {
    asm volatile("svc #1");
  }
  else if(strncmp(command, "run", 4) == 0) {
    void* addr = cpio_get_file_address("user_program.elf", 15);
    printf("addr: %x\n", addr);
    //from el1 to el0 and jump to user_program
    asm volatile("mov x0, #0x3c0\n" "msr spsr_el1, x0\n");
    asm volatile("mov x0, %0\n" "msr elr_el1, x0\n" "eret\n"::"r"(addr + 0x40));
  }
  else if(strncmp(command, "el12el0", 8) == 0) {
    //from el1 to el0
    asm volatile("mov x0, #0\n" "msr spsr_el1, x0\n");
    asm volatile("mov x0, %0\n" "msr elr_el1, x0\n" "eret\n"::"r"((void*)shell));
  }
  else if(strncmp(command, "asyncw", 6) == 0) {
    uart_async_write(command + 6, strlen(command) - 6);
  }
  else if(strncmp(command, "asyncr", 7) == 0) {
    int count = uart_async_read(command, SHELL_COMMNAD_SIZE);
    printf("%s", command);
    printf("read %d bytes\n", count);
  }
  else if(strncmp(command, "settimeout", 10) == 0) {
    core_timer_queue_push(null, strtol(command + 10, 0, 10));
  }
  else {
    printf("unknown command\n");
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
  size_t img_end = load_address + img_size + img_size % 16;
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
  asm volatile ("mov x0, %0\n" "mov x1, %1\n" "mov sp, %2\n" "mov x2, %3\n" "blr %4\n"::
  "r" (load_address),
  "r" (img_size),
  "r" (load_address),
  "r" (get_dtb_address()),
	"r" (relocated_readimg_jump): "x0", "x1", "x2");
}
//read kernel img, and jump 
void readimg_jump(size_t load_address, size_t img_size, size_t dtb_address) {
  uart_read((char* )load_address, img_size);
  asm volatile ("mov x0, %0\n" "mov sp, %1\n" "blr %2\n"::
  "r" (dtb_address),
  "r" (load_address),
	"r" (load_address): "x0");
  //((void (*)(void))(load_address))();
}
