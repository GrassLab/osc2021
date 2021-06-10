# include "user_lib.h"

void main(){
  chdir("sd");
  char buf[100];
  int fd = open("SUBDIR/TEST.TXT", O_RD);
  read(fd, buf, 100);
  buf[99] = '\0';
  uart_write(buf, str_len(buf));
  close(fd);
  exit();
}
