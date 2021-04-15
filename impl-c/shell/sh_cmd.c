#include "shell/cmd.h"

#include "cfg.h"
#include "cpio.h"
#include "string.h"
#include "timer.h"
#include "uart.h"

#include <stddef.h>
#include <stdint.h>

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)0x3F10001c)
#define PM_WDOG ((volatile unsigned int *)0x3F100024)

static void cmdHello();
static void cmdLs();
static void cmdHelp();
static void cmdLoadUser();
static void cmdReboot();

Cmd cmdList[5] = {
    {.name = "hello", .help = "Greeting", .func = cmdHello},
    {.name = "ls", .help = "List files", .func = cmdLs},
    {.name = "load_user",
     .help = "Load and run user program",
     .func = cmdLoadUser},
    {.name = "help", .help = "Show avalible commands", .func = cmdHelp},
    {.name = "reboot", .help = "Reboot device", .func = cmdReboot},
};

Cmd *getCmd(char *name) {
  Cmd *end = cmdList + sizeof(cmdList) / sizeof(Cmd);
  for (Cmd *c = cmdList; c != end; c++) {
    if (!strcmp(c->name, name)) {
      return c;
    }
  }
  return NULL;
}

void cmdHello() { uart_println("Hello!!"); }

void cmdHelp() {
  uart_println("available commands:");
  Cmd *end = cmdList + sizeof(cmdList) / sizeof(Cmd);
  const int minIndent = 10;
  for (Cmd *c = cmdList; c != end; c++) {
    uart_printf("  %s", c->name);
    for (int i = minIndent - strlen(c->name); i > 0; i--) {
      uart_puts(" ");
    }
    uart_println("%s", c->help);
  }
}

void cmdLs() { cpioLs((void *)RAMFS_ADDR); }

void cmdReboot() {
  uart_println("reboot");
  *PM_RSTC = PM_PASSWORD | 0x20;
  *PM_WDOG = PM_PASSWORD | 100; // reboot after 100 watchdog ticks
}

void cmdLoadUser() {
  uart_println("load user program");
  unsigned long size;
  unsigned char *load_addr = (unsigned char *)0x20000000;
  uint8_t *file =
      (uint8_t *)cpioGetFile((void *)RAMFS_ADDR, "./user_program", &size);
  if (file == NULL) {
    uart_println("Cannot found `user_program.o` under rootfs");
    return;
  }
  if (CFG_LOG_ENABLE) {
    uart_println("  [fetchFile] file addr:%x , size:%d", file, size);
  }
  for (unsigned long i = 0; i < size; i++) {
    load_addr[i] = file[i];
  }
  uart_println("start user app");

  // change exception level
  // asm volatile("mov x0, 0x3c0  \n"); // disable timer interrupt, enable svn
  asm volatile("mov x0, 0x340  \n"); // enable core timer interrupt
  asm volatile("msr spsr_el1, x0  \n");
  asm volatile("msr elr_el1, %0   \n" ::"r"(load_addr));
  asm volatile("msr sp_el0, %0    \n" ::"r"(load_addr));

  // enable the core timer’s interrupt in el0
  timer_el0_enable();
  timer_el0_set_timeout();

  // unmask timer interrupt
  asm volatile("mov x0, 2             \n");
  asm volatile("ldr x1, =0x40000040   \n");
  asm volatile("str w0, [x1]          \n");

  asm volatile("eret              \n");
}