# include "uart.h"
# include "utli.h"
# include "cpio.h"
# include "my_math.h"
# include "my_string.h"
# include "buddy.h"
# include "shell.h"
# include "mem.h"
# include "bitset.h"


int main(){
  uart_init();
  buddy_init();
  buddy_dma_init();
  mem_init();
  uart_puts("Hi!\n");
  uart_puts("Welcome to Eric's system ~\n");
  uart_puts("(Lab3)\n");
  uart_flush();
  //buddy_ll_show();
  //
  /*
  int bitset[2];
  char bit_ct[10];
  uart_puts("check a\n");
  bitset_clrall<16>(bitset);
  uart_puts("check a\n");
  bitset_show<16>(bitset);
  uart_puts("check a\n");
  uart_puts("\n");
  for (int i=0; i<64; i++){
    bitset_set<64>(bitset, i);
    bitset_show<64>(bitset);
    uart_puts(" ");
    int_to_str(*bitset, bit_ct);
    uart_puts(bit_ct);
    uart_puts("\n");
  }
  for (int i = 10; i>=0; i-=3){
    bitset_clr<16>(bitset, i);
    bitset_show<16>(bitset);
    uart_puts(" ");
    int_to_str(*bitset, bit_ct);
    uart_puts(bit_ct);
    uart_puts("\n");
  }
  int bit_t = bitset_get_first_one<16>(bitset);
  int_to_str(bit_t, bit_ct);
  uart_puts(bit_ct);
  uart_puts(" ");
  bit_t = bitset_get_first_zero<16>(bitset);
  int_to_str(bit_t, bit_ct);
  uart_puts(bit_ct);
  uart_puts("\n");
  bitset_clrall<64>(bitset);
  bitset_set<64>(bitset, 33);
  bit_t = bitset_get_first_one<64>(bitset);
  int_to_str(bit_t, bit_ct);
  uart_puts(bit_ct);
  uart_puts(" ");
  bit_t = bitset_get_first_zero<64>(bitset);
  int_to_str(bit_t, bit_ct);
  uart_puts(bit_ct);
  uart_puts("\n");
  */
  //buddy_dma_alloc(1, 0);
  //buddy_dma_alloc(18, 0);

  char cmd[1000];
  cmd[0] = '\0';
  int cmd_end = 0;

  //char get_c[10];

  while(1){
    uart_puts("\r> ");
    uart_puts(cmd);
    char c = uart_read();

    // for debug
    //int_to_str((int)c, get_c);
    //uart_puts(get_c);
    //uart_puts("\n");

    if (c == '\n'){
      uart_puts("\n");
      cmd[cmd_end] = '\0';
      invoke_cmd(cmd);
      cmd_end = 0;
      cmd[0] = '\0';
    }
    else if ((int)c == 127 || (int)c == 8){
      cmd_end--;
      cmd[cmd_end] = '\0';
      uart_puts("\b \b");
    }
    else{
      cmd[cmd_end] = c;
      cmd_end++;
      cmd[cmd_end] = '\0';
    }
  }
  return 0;
}
