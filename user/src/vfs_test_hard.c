// #include <assert.h>

#include "printf.h"
#include "syscall.h"
#include "utils.h"

int main(int argc, char **argv) {
  printf("Program: %s, pid: %d\n", argv[0], getpid());

  char buf[8];
  mkdir("mnt");
  int fd = open("/mnt/a.txt", O_CREAT);
  write(fd, "Hi", 2);
  close(fd);
  chdir("mnt");
  fd = open("./a.txt", 0);
  // assert(fd >= 0);
  if (!(fd >= 0)) {
    printf("assert 1 failed\n");
    return 0;
  }
  read(fd, buf, 2);
  // assert(strncmp(buf, "Hi", 2) == 0);
  if (!(strncmp(buf, "Hi", 2) == 0)) {
    printf("assert 2 failed\n");
    return 0;
  }

  chdir("..");
  mount("tmpfs", "mnt", "tmpfs");
  fd = open("mnt/a.txt", 0);
  // assert(fd < 0);
  if (!(fd < 0)) {
    printf("assert 3 failed\n");
    return 0;
  }

  umount("/mnt");
  fd = open("/mnt/a.txt", 0);
  // assert(fd >= 0);
  if (!(fd >= 0)) {
    printf("assert 4 failed\n");
    return 0;
  }
  read(fd, buf, 2);
  // assert(strncmp(buf, "Hi", 2) == 0);
  if (!(strncmp(buf, "Hi", 2) == 0)) {
    printf("assert 5 failed\n");
    return 0;
  }

  return 0;
}