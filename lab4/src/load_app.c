# include "load_app.h"
# include "uart.h"
# include "my_math.h"

extern unsigned char _exec_app_img;

__attribute__((section(".exec.app"))) void load_app(void* load_addr, unsigned long app_size){
  unsigned char *app_entry = (unsigned char *) &_exec_app_img;
  unsigned char *file_head = (unsigned char *) load_addr;

  while(app_size--){
    *app_entry = *file_head;
    app_entry++;
    file_head++;
  }

  int (*app_start)(void) = (int (*)()) &_exec_app_img;
  app_start();
  //char ct[20];
  //int_to_str(r, ct);
  //uart_puts(ct);
  //uart_puts("\n");
}
