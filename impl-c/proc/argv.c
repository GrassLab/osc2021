#include "mm.h"
#include "string.h"
#include "uart.h"

#include "cfg.h"
#include "log.h"
#include "test.h"

#include <stdint.h>

#ifdef CFG_LOG_PROC_ARGV
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

// sp value must be 16 bytes aligned
#define SP_ALIGN 16

static inline uintptr_t align_up(uintptr_t n, unsigned long align) {
  return (n + align - 1) & (~(align - 1));
}

/**
 * @brief Place argv into user stack
 * @param src_sp the original user sp value
 * @param src_argv argv to copy from
 * @param ret_argc (total argc count)
 * @param ret_argv (alt)
 * @param ret_sp new sp value
 */
void place_args(/*IN*/ uintptr_t src_sp, /*IN*/ const char **src_argv,
                /*OUT*/ int *ret_argc,
                /*OUT*/ char ***ret_argv,
                /*OUT*/ uintptr_t *ret_sp) {

  log_println("[place_arg] src_sp:%x", src_sp);

  int nm_args = 0;
  while (src_argv[nm_args] != NULL) {
    log_println(" src_argv[%d]:`%s`", nm_args, src_argv[nm_args]);
    nm_args++;
  }

  if (ret_argc != NULL) {
    *ret_argc = nm_args;
  }

  // Save Arguments into kernel memory.
  // We need to backup argv since they are stored in the user stack,
  //  which would be overwritten by following operations.
  char **saved_args = kalloc(sizeof(char *) * nm_args);
  char *arg_str = NULL;

  for (int i = 0; i < nm_args; i++) {
    arg_str = kalloc(sizeof(char) * (strlen(src_argv[i]) + 1));
    strcpy(arg_str, src_argv[i]);
    saved_args[i] = arg_str;
  }

  /**
   * Save argv into user stack.
   *
   * High
   * | "str2"  | <- `args_offset[2]` / old stack top
   * | "str1"  | <- `args_offset[1]`
   * | "str0"  | <- `args_offset[0]`
   * | argv[2] |
   * | argv[1] |
   * | argv[0] |
   * |stack_top| <- new user stack
   * Low
   */

  // Calculate total size to store in user stack
  // size is calculated in byte
  size_t *args_offset = kalloc(sizeof(size_t) * nm_args);
  size_t size_byte = 0, _size = 0;

  // Size for **argv array
  _size = sizeof(char **) * nm_args;
  size_byte += _size;
  args_offset[0] = size_byte;
  log_println("[place_arg] size for argv array: %d(byte)", _size);

  // Size for each argv string
  for (int i = 0; i < nm_args; i++) {
    _size = strlen(saved_args[i]) + 1;
    size_byte += _size;
    args_offset[i + 1] = size_byte;
    log_println("[place_arg] size for src_argv[%d]: %d(byte)", i, _size);
  }

  for (int i = 0; i < nm_args; i++) {
    log_println(" args_offset[%d] -> %d", i, args_offset[i]);
  }

  log_println("total bytes -> %d", size_byte);

  // The new user sp value (byte-addressable)
  {
    size_t aligned = align_up(size_byte, SP_ALIGN);
    log_println("[place arg] total bytes:%d, after alignment:%d", size_byte,
                aligned);
    size_byte = aligned;
  }

  // 1. sp grow from HIGH->LOW
  // 2. sp point to an empty memory space
  int base = SP_ALIGN;

  uintptr_t sp = src_sp;
  sp -= size_byte;
  log_println("[place arg] move sp(%x) -> sp(%x)", src_sp, sp);

  // Calculate directly in byte address
  char **user_argv = (char **)(sp + base);
  for (int i = 0; i < nm_args; i++) {
    user_argv[i] = (char *)(sp + base + args_offset[i]);
  }

  // Copy args to user stack (would overwrite existing data)
  for (int i = 0; i < nm_args; i++) {
    strcpy((char *)(sp + base + args_offset[i]), saved_args[i]);
    log_println("[place arg] argv[%d](%x) written -> %s", i,
                sp + base + args_offset[i], sp + base + args_offset[i]);
    // log_println("src_argv[%d] => %x", i, saved_args[i]);
  }

  if (ret_argv != NULL) {
    *ret_argv = (char **)(sp + base);
  }
  if (ret_sp != NULL) {
    *ret_sp = sp;
  }

  // Caution: need to be freed.
  kfree(args_offset);
  for (int i = 0; i < nm_args; i++) {
    kfree(saved_args[i]);
  }
  kfree(saved_args);
}

#ifdef CFG_RUN_PROC_ARGV_TEST
bool test_good() {
  char *src_argv[] = {"./hello_world.out", "--name", "ian", NULL};
  char stack[400];

  uintptr_t src_sp = align_up((uintptr_t)&stack[350], SP_ALIGN);
  uintptr_t new_sp;

  char **new_argv;
  int argc;

  place_args(src_sp, (const char **)src_argv, &argc, &new_argv, &new_sp);
  assert((new_sp % SP_ALIGN) == 0);
  assert(new_sp < src_sp);
  assert(argc == 3);
  for (size_t i = 0; i < argc; i++) {
    assert((new_sp < (uintptr_t)new_argv[i]));
    // sp point to an empty memory space
    assert(((uintptr_t)new_argv[i] < (src_sp + SP_ALIGN)));
  }
  return true;
}
#endif

void test_argv_parse() {
#ifdef CFG_RUN_PROC_ARGV_TEST
  unittest(test_good, "proc", "argv");
#endif
}