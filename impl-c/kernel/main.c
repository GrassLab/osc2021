#include "config.h"
#include "dev/sd.h"
#include "fatal.h"
#include "fs/fat.h"
#include "fs/tmpfs.h"
#include "fs/vfs.h"
#include "mm.h"
#include "mm/startup.h"
#include "proc.h"
#include "shell/shell.h"
#include "test.h"
#include "uart.h"

#include "string.h"

#define ANSI_GREEN(s) ("\033[0;32m" s "\033[0m")

#define MX_CMD_BFRSIZE 64
extern unsigned char __kernel_start, __kernel_end;

static void init_sys(char *name, void (*func)(void));
static inline void run_shell();
static void reserve_startup_area();

static void tmpfs_lab7_demo() {
  {
    char *fname = "folder/test.txt";
    char *h = "Hello";
    char *w = "      World!";
    char buf[100] = {0};
    struct file *f0 = vfs_open(fname, FILE_O_CREAT);
    struct file *f1 = vfs_open(fname, FILE_O_CREAT);
    struct file *f2 = vfs_open(fname, FILE_O_CREAT);
    struct file *f3 = vfs_open(fname, FILE_O_CREAT);
    vfs_read(f0, buf, 30);
    uart_println("Original content of `%s`: %s", fname, buf);

    vfs_write(f1, w, strlen(w));
    vfs_write(f2, h, strlen(h));
    vfs_read(f3, buf, 30);
    uart_println("After write: %s", buf);
    vfs_close(f0), vfs_close(f1), vfs_close(f2), vfs_close(f3);
  }
}

static void fat_lab8_demo() {
  // test read from fatfs
  {
    char buf[100] = {0};
    struct file *f = vfs_open("dev/sdcard/KERNEL8.IMG", 0);
    vfs_read(f, buf, 100);
    uart_println("First 100 bytes of /dev/sdcard/KERNEL8.IMG");
    for (int row = 0; row < 10; row++) {
      uart_printf("%d-%d", (row * 10), ((row + 1) * 10 - 1));
      for (int i = 0; i < 10; i++) {
        uart_printf("\t %x", buf[(row * 10) + i]);
      }
      uart_printf("\n");
    }
    vfs_close(f);
  }

  // Hello world
  {
    char *fname = "dev/sdcard/hello.txt";
    char *h = "Hello";
    char *w = "      World!";
    char buf[100] = {0};
    struct file *f1 = vfs_open(fname, FILE_O_CREAT);
    struct file *f2 = vfs_open(fname, FILE_O_CREAT);
    struct file *f3 = vfs_open(fname, FILE_O_CREAT);
    vfs_write(f1, w, strlen(w));
    vfs_write(f2, h, strlen(h));
    vfs_read(f3, buf, 30);
    uart_println("Result 2: %s", buf);
    vfs_close(f1), vfs_close(f2), vfs_close(f3);
  }
}

/**
 * Kernel main function
 * Power up the whole system
 */
void main() {
  uart_init();
  uart_println("uart initialized");

  init_sys("Init Startup allocator", startup_init);
  init_sys("Reserve memory area", reserve_startup_area);
  init_sys("Init Memory Allocator", KAllocManager_init);

  // KAllocManager_run_example();
  // KAllocManager_show_status();

  init_sys("Init Proc subsystem", proc_init);
  init_sys("SD card driver", sd_init);

#ifdef CFG_RUN_TEST
  run_tests();
#endif

  vfs_init();
  register_filesystem(&tmpfs);
  register_filesystem(&fat);
  mount_root_fs("tmpfs");

  // mount SD card to our filesystem tree
  struct vnode *sdcard_root = NULL;
  vfs_find_vnode("/dev", true);
  sdcard_root = vfs_find_vnode("/dev/sdcard", true);
  if (0 != mount(sdcard_root, "fat32")) {
    FATAL("Could not mount sd card");
  }

  tmpfs_lab7_demo();
  fat_lab8_demo();
  // FATAL("Done");

  test_tasks();
  // run_shell();
}
// =====

void init_sys(char *name, void (*func)(void)) {
  uart_printf("%s ...", name);
  func();
  uart_println("\r%s ... %s", name, ANSI_GREEN("success"));
}

void reserve_startup_area() {
  // Kernel
  startup_reserve((void *)0x0, 0x1000);      // spin table
  startup_reserve((void *)0x60000, 0x20000); // stack
  startup_reserve((void *)(&__kernel_start),
                  (&__kernel_end - &__kernel_start)); // kernel
  // startup_reserve((void *)(&kn_end), mem_size / PAGE_SIZE);    // buddy
  // System
  startup_reserve((void *)0x3f000000, 0x1000000); // MMIO
}

void run_shell() {
  struct Shell sh;
  char shell_buffer[MX_CMD_BFRSIZE + 1];
  shell_init(&sh, shell_buffer, MX_CMD_BFRSIZE);

  uart_println("-------------------------------");
  uart_println(" Operating System Capstone 2021");
  uart_println("-------------------------------");
  uart_println(" input filename to see file content");
  while (1) {
    shell_show_prompt(&sh);
    shell_input_line(&sh);
    shell_process_command(&sh);
  }
}