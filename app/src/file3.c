# include "user_lib.h"

void main(){
  char buf[8];
  mkdir("mnt");
  int fd = open("/mnt/a.txt", O_CREAT | O_WR);
  write(fd, "Hi", 2);
  close(fd);
  chdir("mnt");
  fd = open("./a.txt", O_RD);
  assert(fd >= 0, "A");
  read(fd, buf, 2);
  assert(str_cmp(buf, "Hi") == 1, "B");

  chdir("..");
  mount("tmpfs", "mnt", "tmpfs");
  fd = open("mnt/a.txt", 0);
  assert(fd < 0, "C");

  umount("/mnt");
  fd = open("/mnt/a.txt", 0);
  assert(fd >= 0, "D");
  read(fd, buf, 2);
  assert(str_cmp(buf, "Hi") == 1, "E");
  exit();
}
