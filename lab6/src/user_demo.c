# include "user_demo.h"
# include "user_lib.h"

//# include "uart.h"

const char *argv_test_argv[] = {"argv_test", "-o", "argv2", "argv3", "Hello~~~", 0};
const char *fork_test_argv[] = {"fork_test", 0};

void fork_test(int argc, char **argv){
  int cnt = 1;
  int ret = 0;
  char ct[20];
  uart_write((char *) "pid: ", 5);
  int_to_str(getpid(), ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) ", cnt: ", 7);
  int_to_str(cnt, ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) ", &cnt: ", 8);
  ptr_to_hex((void *)(&cnt), ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) "\n", 1);
  if ((ret = fork()) == 0){
    uart_write((char *) "pid: ", 5);
    int_to_str(getpid(), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", cnt: ", 7);
    int_to_str(cnt, ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", &cnt: ", 8);
    ptr_to_hex((void*)(&cnt), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) "\n", 1);
    cnt++;
    fork();
    while(cnt < 5){
      uart_write((char *) "pid: ", 5);
      int_to_str(getpid(), ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) ", cnt: ", 7);
      int_to_str(cnt, ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) ", &cnt: ", 8);
      ptr_to_hex((void *)(&cnt), ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) "\n", 1);
      delay(1000000);
      cnt++;
    }
  }
  else{
    uart_write((char *) "parent here, pid ", 17);
    int_to_str(getpid(), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", child ", 8);
    int_to_str(ret, ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) "\n", 1);
    delay(10000000);
  }
  exit();
}


void argv_test(int argc, char **argv){
  char ct[20];
  uart_write((char *) "Argv Test, pid: ", 16);
  int_to_str(getpid(), ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) "\n", 1);
  uart_write((char *) "argc: ", 5);
  int_to_str(argc, ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) "\n", 1);
  for (int i=0; i<argc; i++){
    uart_write(argv[i], str_len(argv[i]));
    uart_write((char *)"\n", 1);
  }
  exec(fork_test, (char **)fork_test_argv);
  exit();
}


void user_demo_test(){
  exec(argv_test, (char **)argv_test_argv);
  exit();
}

void file_demo_1(){
  char buf[110];
  //uart_write((char *) "File Test", 16);
  //uart_write((char *) "\n", 1);
  int a = open("tt/hello", O_CREAT | O_WR);
  int b = open("tt/world", O_CREAT | O_WR);
  write(a, "Hello ", 6);
  write(b, "World!", 6);
  close(a);
  close(b);
  //uart_write((char *) "write done\n", 16);
  b = open("tt/hello", O_RD);
  a = open("tt/world", O_RD);
  int sz;
  sz = read(b, buf, 100);
  sz += read(a, buf + sz, 100);
  buf[sz] = '\0';
  uart_write(buf, sz+1);
  uart_write((char *) "\n", 1);
  //printf("%s\n", buf);
  //uart_write((char *) "File exit", 16);
  //uart_write((char *) "\n", 1);
  exit();
}

void file_demo_2(){
  char buf[110];
  //uart_write((char *) "File Test", 16);
  //uart_write((char *) "\n", 1);
  int b = open("hello", O_RD);
  int a = open("world", O_RD);
  int sz;
  sz = read(b, buf, 100);
  sz += read(a, buf + sz, 100);
  buf[sz] = '\0';
  uart_write(buf, sz+1);
  //printf("%s\n", buf);
  //uart_write((char *) "File exit", 16);
  //uart_write((char *) "\n", 1);
  exit();
}

void file_demo_3(){
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

void file_demo_4(){
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
  mkdir("test");
  mount("tmpfs", "test", "tmpfs");
  fd = open("test/a.txt", O_RD);
  assert(fd < 0, "C");

  umount("/mnt");
  fd = open("/mnt/a.txt", 0);
  assert(fd >= 0, "D");
  read(fd, buf, 2);
  assert(str_cmp(buf, "Hi") == 1, "E");
  exit();
}
