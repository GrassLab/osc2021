#ifndef __SDCARD_H_
#define __SDCARD_H_

void readblock(int block_idx, void* buf);
void writeblock(int block_idx, void* buf);
void sd_init();

#endif