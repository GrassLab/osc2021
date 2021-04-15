#pragma once

#include "cfg.h"

struct InputBuffer {
  char *data;
  int mx_size;
  int write_head;
  int cur_input_size;
};
void bfr_init(struct InputBuffer *bfr, char *data, int mx_size);
void bfr_pop(struct InputBuffer *bfr);
void bfr_push(struct InputBuffer *bfr, char c);
void bfr_cursor_mov_left(struct InputBuffer *bfr);
void bfr_cursor_mov_right(struct InputBuffer *bfr);
void bfr_clear(struct InputBuffer *bfr);

// Only used for running tests
void test_shell_buffer();