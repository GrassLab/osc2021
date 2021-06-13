#include "proc/exec.h"
#include "shell/cmd.h"

#include "bool.h"
#include "config.h"
#include "dev/cpio.h"
#include "log.h"
#include "string.h"
#include "test.h"
#include "timer.h"
#include "uart.h"

#include <stddef.h>
#include <stdint.h>

// #ifdef CFG_LOG_SHELL_CMD
// static const int _DO_LOG = 1;
// #else
// static const int _DO_LOG = 0;
// #endif

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)0x3F10001c)
#define PM_WDOG ((volatile unsigned int *)0x3F100024)

static void cmdHello();
static void cmdLs();
static void cmdHelp();
// static void cmdLoadUser();
static void cmdReboot();

Cmd cmdList[] = {
    {.name = "hello", .help = "Greeting", .func = cmdHello},
    {.name = "ls", .help = "List files", .func = cmdLs},
    // {.name = "load_user",
    //  .help = "Load and run user program",
    //  .func = cmdLoadUser},
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

  // Determine the indent length
  int mx_cmd_len = 0, tmp = 0;
  for (Cmd *c = cmdList; c != end; c++) {
    tmp = strlen(c->name);
    mx_cmd_len = tmp > mx_cmd_len ? tmp : mx_cmd_len;
  }
  int minIndent = mx_cmd_len + 2;

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

// TODO: make this work with scheduling
// void cmdLoadUser() {
//   log_println("load user program");
//   const char *argv[] = {NULL};
//   exec("./get_pid.out", argv);
// }

#ifdef CFG_RUN_SHELL_CMD_TEST
bool test_cmd_get() {
  char *bfr1 = "help";
  Cmd *c;
  c = getCmd(bfr1);
  assert(c->func == cmdHelp);
  return true;
}

bool test_cmd_get_null() {
  char *bfr1 = "help ";
  assert(getCmd(bfr1) == NULL);

  char *bfr2 = "";
  assert(getCmd(bfr2) == NULL);
  return true;
}

#endif

void test_shell_cmd() {
#ifdef CFG_RUN_SHELL_CMD_TEST
  unittest(test_cmd_get, "shell", "cmd get1");
  unittest(test_cmd_get_null, "shell", "cmd get2");
#endif
}