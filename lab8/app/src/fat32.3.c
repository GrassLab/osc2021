# include "user_lib.h"

void main(){
  chdir("sd/SUBDIR");
  int fd = open("新文字文件.txt", O_RD);
  assert(fd>=0, "File open fail");
  char buf[100];
  read(fd, buf, 100);
  buf[99] = '\0';
  uart_write(buf, str_len(buf));
  close(fd);
  exit();
}
