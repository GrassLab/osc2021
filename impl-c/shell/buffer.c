#include "shell/buffer.h"
#include "bool.h"
#include "cfg.h"
#include "test.h"

void bfr_init(struct InputBuffer *bfr, char *data, int mx_size) {
  bfr->data = data;
  bfr->mx_size = mx_size;
  bfr->write_head = 0;
  bfr->cur_input_size = 0;
  bfr->data[bfr->cur_input_size] = 0;
}

void bfr_cursor_mov_left(struct InputBuffer *bfr) {
  if (bfr->write_head > 0) {
    bfr->write_head--;
  }
}

void bfr_cursor_mov_right(struct InputBuffer *bfr) {
  if (bfr->write_head < bfr->cur_input_size) {
    bfr->write_head++;
  }
}

void bfr_push(struct InputBuffer *bfr, char c) {
  if (bfr->cur_input_size >= bfr->mx_size) {
    // buffer is full
    return;
  }
  if (bfr->write_head <= bfr->cur_input_size) {
    // Insert in middle: right shift buffer first
    for (int i = bfr->cur_input_size; i > bfr->write_head; i--) {
      bfr->data[i] = bfr->data[i - 1];
    }
    bfr->data[bfr->write_head++] = c;
    bfr->cur_input_size++;
    bfr->data[bfr->cur_input_size] = 0;
  }
}

void bfr_pop(struct InputBuffer *bfr) {
  if (bfr->write_head > 0) {
    bfr->write_head--;
    // left shift the whole buffer
    for (int i = bfr->write_head; i < bfr->cur_input_size; i++) {
      bfr->data[i] = bfr->data[i + 1];
    }
    bfr->data[bfr->cur_input_size--] = 0;
  }
}

void bfr_clear(struct InputBuffer *bfr) {
  bfr->cur_input_size = 0;
  bfr->write_head = 0;
  bfr->data[0] = 0;
}

//============== TEST START =================
#ifdef CFG_RUN_SHELL_BUFFER_TEST
bool test_bfr_init() {
  char data[10] = {'a', 'b', 'c'};
  struct InputBuffer bfr;
  bfr_init(&bfr, data, 10);
  assert(data[0] == 0);
  return true;
}

bool test_bfr_write() {
  char data[10];
  struct InputBuffer bfr;
  bfr_init(&bfr, data, 10);
  bfr_push(&bfr, 'b');
  bfr_push(&bfr, 'c');
  bfr_cursor_mov_left(&bfr);
  bfr_cursor_mov_left(&bfr);
  bfr_push(&bfr, 'a');
  assert(data[0] == 'a');
  assert(data[1] == 'b');
  assert(data[2] == 'c');
  return true;
}

bool test_bfr_clear() {
  char data[10];
  struct InputBuffer bfr;
  bfr_init(&bfr, data, 10);
  bfr_push(&bfr, 'b');
  bfr_push(&bfr, 'c');
  bfr_clear(&bfr);
  assert(data[0] == 0);
  assert(bfr.write_head == 0);
  return true;
}

bool test_bfr_pop() {
  char data[10];
  struct InputBuffer bfr;
  bfr_init(&bfr, data, 10);
  bfr_push(&bfr, 'b');
  bfr_push(&bfr, 'c');
  bfr_cursor_mov_left(&bfr);
  bfr_pop(&bfr);
  assert(data[0] == 'c');
  return true;
}

#endif

void test_shell_buffer() {
#ifdef CFG_RUN_SHELL_BUFFER_TEST
  unittest(test_bfr_init, "shell", "buffer init");
  unittest(test_bfr_write, "shell", "buffer write");
  unittest(test_bfr_clear, "shell", "buffer clear");
  unittest(test_bfr_pop, "shell", "buffer delete");

#endif
}