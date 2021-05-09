#pragma once

#include "stdint.h"

/**
 * Trap Frame
 * Once entering system via exception, the whole cpu context just before
 * exception would be store into memory (which should be pointed by a pointer to
 * `trap_frame` struct). Returning to user code means restoring cpu values from
 * a trap frame. Also, changing the value of the trap frame would change user
 * context after returning from system (which is a good way to impl syscall)
 */
struct trap_frame {
  uint64_t regs[32];
};