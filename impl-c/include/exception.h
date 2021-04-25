#pragma once

#include "stdint.h"

struct trap_frame {
  uint64_t regs[32];
};