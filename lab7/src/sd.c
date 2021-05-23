# include "sd.h"
# include "sdhost.h"
# include "uart.h"
# include "my_math.h"

void listblock(int n){
  char buf[SECTOR_SIZE];
  readblock(n, buf);
  char data[SECTOR_TABLE_OFFSET+SECTOR_TABLE_COLS*4+3];
  for (int i=0; i<SECTOR_TABLE_OFFSET+1; i++){
    data[i] = ' ';
  }
  for (int c=0; c<SECTOR_TABLE_COLS; c++){
    int msb = c/16;
    int lsb = c%16;
    data[c*4+SECTOR_TABLE_OFFSET+1] = (c%8 == 0) ? '|' : ' ';
    data[c*4+SECTOR_TABLE_OFFSET+2] = TO_HEX(msb);
    data[c*4+SECTOR_TABLE_OFFSET+3] = TO_HEX(lsb);
    data[c*4+SECTOR_TABLE_OFFSET+4] = ' ';
  }
  data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+1] = '|';
  data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+2] = '\0';
  uart_puts(data);
  uart_puts("\n");
  for (int i=0; i<SECTOR_TABLE_OFFSET+SECTOR_TABLE_COLS*4+2; i++){
    data[i] = '=';
  }
  data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+2] = '\0';
  uart_puts(data);
  uart_puts("\n");
  data[SECTOR_TABLE_OFFSET] = ' ';
  for (int r=0; r<SECTOR_TABLE_ROWS; r++){
    int_to_hex_align(n*SECTOR_SIZE+r*SECTOR_TABLE_COLS, data, SECTOR_TABLE_OFFSET-1);
    for (int c=0; c<SECTOR_TABLE_COLS; c++){
      int msb = ((int)buf[r*SECTOR_TABLE_COLS+c])/16;
      int lsb = ((int)buf[r*SECTOR_TABLE_COLS+c])%16;
      data[c*4+SECTOR_TABLE_OFFSET+1] = (c%8 == 0) ? '|' : ' ';
      data[c*4+SECTOR_TABLE_OFFSET+2] = TO_HEX(msb);
      data[c*4+SECTOR_TABLE_OFFSET+3] = TO_HEX(lsb);
      data[c*4+SECTOR_TABLE_OFFSET+4] = ' ';
    }
    data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+1] = '|';
    data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+2] = '\0';
    uart_puts(data);
    uart_puts("\n");
  }
  for (int i=0; i<SECTOR_TABLE_OFFSET+SECTOR_TABLE_COLS*4+2; i++){
    data[i] = '=';
  }
  data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+2] = '\0';
  uart_puts(data);
  uart_puts("\n");
}
