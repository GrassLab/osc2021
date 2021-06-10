#ifndef __SDHOST_H
#define __SDHOST_H

void readblock(int block_idx, void* buf);
void writeblock(int block_idx, void* buf);
void sd_init();

#endif
