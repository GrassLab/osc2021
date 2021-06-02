#include "test.h"
#include "config.h"
#include "dev/mbr.h"
#include "fs/fat.h"
#include "fs/vfs.h"
#include "mm/startup.h"
#include "proc/argv.h"
#include "shell/buffer.h"
#include "shell/cmd.h"

#include "string.h"

void run_tests() {

  // lib
  test_string();

  // components
  test_startup_alloc();
  test_shell_buffer();
  test_shell_cmd();
  test_argv_parse();
  test_vfs();
  test_mbr();
  test_fat();
}