#pragma once

#include "buffer.h"
#include <stdint.h>

struct Shell {
  char *data;
  uint32_t bfr_size;
  struct InputBuffer bfr;
};

void shell_show_prompt(struct Shell *sh);
void shell_input_line(struct Shell *sh);
void shell_process_command(struct Shell *sh);
void shell_init(struct Shell *sh, char *data, uint32_t size);