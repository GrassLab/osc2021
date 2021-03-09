#include "shell.h"

void shell() {
  uart_puts("*****************************Hello World*****************************\n");
  uart_puts("% ");

  char command[256];
  memset(command, '\0');
  int i = 0;
  while(1) {
    char c = uart_getc();
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
        uart_puts("help: print all available commands\n");
        uart_puts("hello: print Hello World!\n");
        uart_puts("reboot: \n");
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
  else {
    uart_puts("unknown command\n");
  }	
}

void loadimg() {
  size_t load_address;
  char buf[9]; 
  size_t img_size;
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
  //read kernel img 
  uart_puts("\nLoad image...\n");
  uart_read((char* )load_address, img_size);
  uart_puts("\nJump to address...\n");
  asm volatile("mov sp, %0" ::"r"(load_address));
  ((void (*)(void))(load_address))();
}



