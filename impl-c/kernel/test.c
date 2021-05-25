#include "test.h"
#include "config.h"
#include "mm/startup.h"
#include "proc/argv.h"
#include "shell/buffer.h"
#include "shell/cmd.h"

void run_tests() {
  test_startup_alloc();
  test_shell_buffer();
  test_shell_cmd();
  test_argv_parse();
}