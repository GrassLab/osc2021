#include "start.h"

int main(int argc, char **argv) {
  print_s("VFS Test, pid ");
  print_i(getpid());
  print_s("\n");

  int a = open("hello", O_CREAT);
  int b = open("world", O_CREAT);
  if (a >= 0 && b >= 0) {
    write(a, "Hello ", 6);
    write(b, "World!", 6);
    close(a);
    close(b);
  }
  b = open("hello", 0);
  a = open("world", 0);
  if (a >= 0 && b >= 0) {
    int sz;
    char buf[105];
    sz = read(b, buf, 100);
    sz += read(a, buf + sz, 100);
    buf[sz] = '\0';
    // printf("%s\n", buf); // should be Hello World!
    print_s(buf);
    print_s("\n");
  }

  print_s("\nfile1.txt\n");
  a = open("file1.txt", 0);
  if (a >= 0) {
    int sz;
    char buf[200];
    sz = read(a, buf, 100);
    buf[sz] = '\0';
    print_s(buf);
    print_s("\n");
  }
  close(a);

  print_s("\nfile2.txt\n");
  a = open("file2.txt", 0);
  if (a >= 0) {
    int sz;
    char buf[200];
    sz = read(a, buf, 100);
    buf[sz] = '\0';
    print_s(buf);
    print_s("\n");
  }
  close(a);

  return 0;
}