#include "kernel.h"
void shell();
/*  do_funcs */
void do_reset(int tick) {         // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;  // full reset
  *PM_WDOG = PM_PASSWORD | tick;  // number of watchdog tick
}
void do_help() {
  printf("help: print available commands\n");
  printf("reboot: restart device\n");
  printf("ls: list file\n");
  printf("cat: print file context\n");
  printf("clear: clean screen\n");
}
void do_except(char *buff) { printf("No command: %s\n", buff); }
void do_ls() {
  char *now_addr = (char *)CPIO_ARRD, *filename, *context;
  struct cpio_newc_header *cpio_header;
  while (!strcmp("TRAILER!!!", filename = now_addr + CPIO_SIZE)) {
    printf("%s\n", filename);
    cpio_info(&cpio_header, &now_addr, &context);
  }
}
void do_cat(char *buff) {
  char *now_addr = (char *)CPIO_ARRD, *filename, *context;
  struct cpio_newc_header *cpio_header;
  int k = 0;
  unsigned long long int context_size = 0;
  while (buff[k] != '\0') k++;
  do {
    filename = now_addr + CPIO_SIZE;
    context_size = cpio_info(&cpio_header, &now_addr, &context);
  } while (!strcmp("TRAILER!!!", filename) && !strcmp(&buff[k + 1], filename));
  /* check file exist */
  if (!strcmp(&buff[k + 1], filename))
    printf("cat: %s : No such file or directory\n", &buff[k + 1]);
  /* get cpio context size */
  for (int i = 0; i < context_size; i++) printf("%c", context[i]);
  printf("\n");
}
void do_clear() { printf("\033c"); }
void do_run(char *buff) {
  // exec("test.img", (char *[]){"test.img", NULL});
}
void do_top() {
  // kill_zombies(&wait_q);
  // print_process_q(process_head);
  // print_thread_q(wait_q);
}
/*
 * lab5 test
 */

void foo4() {
  for (int i = 0; i < 5; ++i) {
    printf("Thread id: %d, i: %d\n", gettid(), i);
    schedule();
  }
  exit_thread();
}
void do_req1() {
  for (int i = 0; i < 5; i++) thread_create(foo4);
  schedule();
}
void user_test() {
  char *argv[] = {"argv_test.img", "-o", "arg2", 0};
  exec("argv_test.img", argv);
  exit();
}
void do_req2() {
  process_create(thread_create(user_test), 0, 0);
  schedule();
}
/*
 * lab6 test
 */

void user_tmpfs() {
  char *argv[] = {"tmpfs_test.img", 0};
  exec("tmpfs_test.img", argv);
  exit();
}
void do_test() {
  process_create(thread_create(user_tmpfs), 0, 0);
  schedule();
}
void user_ls() {
  char *argv[] = {"ls.img", ".", 0};
  exec("ls.img", argv);
  exit();
}
void do_ls2() {
  process_create(thread_create(user_ls), 0, 0);
  schedule();
}
void shell() {
  do_clear();
  char buff[buff_size];
  /* say hello */
  printf(
      "\n"
      "++++++++++++++++++++++\n"
      "+++ Hello Kernel!! +++\n"
      "++++++++++++++++++++++\n");
  while (1) {
    printf("$ ");
    get_cmd(buff);
    if (buff[0] == '\0')
      continue;
    else if (strcmp(buff, "help"))
      do_help();
    else if (strcmp(buff, "ls"))
      do_ls();
    else if (strcmp(buff, "cat"))
      do_cat(buff);
    else if (strcmp(buff, "reboot")) {
      do_reset(100);
      return;
    } else if (strcmp(buff, "clear"))
      do_clear();
    else if (strcmp(buff, "run"))
      do_run(buff);
    else if (strcmp(buff, "req1"))
      do_req1();
    else if (strcmp(buff, "req2"))
      do_req2();
    else if (strcmp(buff, "test"))
      do_test();
    else if (strcmp(buff, "ls2"))
      do_ls2();
    // else if (strcmp(buff, "test3"))
    //   do_test3();
    // else if (strcmp(buff, "test4"))
    //   do_test4();
    else if (strcmp(buff, "top"))
      do_top();
    else
      do_except(buff);
  }
}

void main() {
  uart_init();  // set up serial console
  printf("\nuart_init\n");
  buddy_init((char *)BUDDY_START);
  printf("\nbuddy_init\n");
  dma_init();
  printf("\ndma_init\n");
  timer_init();
  printf("\ntimer_init\n");
  thread_init();
  printf("\nthread_init\n");
  process_init();
  printf("\nprocess_init\n");
  process_init();
  printf("\nprocess_init\n");
  filesystem_init();
  printf("\nfilesystem_init\n");
  shell();
}