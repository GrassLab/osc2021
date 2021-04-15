#include "test.h"
#include "cfg.h"
#include "mm/startup.h"
#include "shell/buffer.h"

void run_tests() {
  test_startup_alloc();
  test_shell_buffer();
}