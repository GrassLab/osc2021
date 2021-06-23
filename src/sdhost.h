#ifndef SDHOST_H
#define SDHOST_H

void sd_init();
void readblock(int block_id, void *buf);
void writeblock(int block_id, void *buf);
void print_block(int block_id);

#endif
