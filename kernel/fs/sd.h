#ifndef SD_H
#define SD_H

void sd_init();

void readblock(int block_idx, void * buf);
void writeblock(int block_idx, void * buf);

#endif