#include <syscall.h>
#include <printf.h>
#include <types.h>
#include <time.h>
/*int main(int argc, char **argv) {
  printf("Argv Test, pid %d\n", getpid());
  for (int i = 0; i < argc; ++i) {
    printf("%s\n", argv[i]);
  }
  char *fork_argv[] = {"fork_test", 0};
  delay(2);
  exec("fork_test", fork_argv);
  return 0;
}*/

/*int main(void) {
    printf("Fork Test, pid %d\n", getpid());
    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) { // child
      printf("pid: %d, cnt: %d, ptr: %p\n", getpid(), cnt, &cnt);
      ++cnt;
      fork();
      while (cnt < 5) {
          printf("pid: %d, cnt: %d, ptr: %p\n", getpid(), cnt, &cnt);
          delay(3);
          ++cnt;
      }
    } else {
        printf("parent here, pid %d, child %d\n", getpid(), ret);
    }
  return 0;
}*/

/*int main() {
  printf("VFS Test\n");
  char buf[256];
  int a = open("hello", O_CREAT);
  int b = open("world", O_CREAT);
  write(a, "Hello ", 6);
  write(b, "World!", 6);
  close(a);
  close(b);
  b = open("hello", 0);
  a = open("world", 0);
  int sz;
  sz = read(b, buf, 100);
  sz += read(a, buf + sz, 100);
  buf[sz] = '\0';
  printf("%s\n", buf); // should be Hello World!
  return 0;
}*/

int main(int argc, char** argv) {
  int fd = open(argv[1], 0);
  char name[100];
  int size;
  // Modify the for loop to iterate the directory entries of the opened directory.
  while(1) {
    size = read(fd, name, 64);
    if(size == 0)
      break;
    printf("Name: %s Size: %d\n", name, size);
  }
  return 0;
}