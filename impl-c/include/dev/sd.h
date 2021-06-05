#pragma once

// SD card driver

void sd_init();

// Read 512 bytes from SD card
void readblock(int block_idx, void *buf);

// Write 512 bytes to SD card
void writeblock(int block_idx, void *buf);