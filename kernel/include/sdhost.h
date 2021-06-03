#pragma once

// static inline void delay(unsigned long tick);
// static void pin_setup();
// static void sdhost_setup();
// static int wait_sd();
// static int sd_cmd(unsigned cmd, unsigned int arg);
// static int sdcard_setup();
// static int wait_fifo();
// static void set_block(int size, int cnt);
// static void wait_finish();
void readblock(int block_idx, void* buf);
void writeblock(int block_idx, void* buf);
void sd_init();
