#ifndef _SDHOST_H
#define _SDHOST_H
void sd_init();
void writeblock(int block_idx, void* buf);
void readblock(int block_idx, void* buf);
#endif