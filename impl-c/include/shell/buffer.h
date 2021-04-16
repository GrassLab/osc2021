#pragma once

struct InputBuffer {
  char *data;
  int mx_size;
  int write_head;
  int cur_input_size;

  void (*pop)(struct InputBuffer *self);
  void (*push)(struct InputBuffer *self, char c);
  void (*cursor_mov_left)(struct InputBuffer *self);
  void (*cursor_mov_right)(struct InputBuffer *self);
  void (*clear)(struct InputBuffer *self);
};

void InputBuffer_init(struct InputBuffer *bfr, char *data, int mx_size);

// Only used for running tests
void test_shell_buffer();