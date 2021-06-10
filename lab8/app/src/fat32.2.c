# include "user_lib.h"

void main(){
  chdir("sd");
  int fd = open("Hi.txt", O_WR);
  char write_data[] = "Write by Rpi~.Write by Rpi~.Write by Rpi~.Write by Rpi~.";
  write(fd, write_data, str_len(write_data));
  close(fd);
  exit();
}
