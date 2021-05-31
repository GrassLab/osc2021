#pragma once

// SD card driver

void sd_init();
void readblock(int block_idx, void *buf);
void writeblock(int block_idx, void *buf);