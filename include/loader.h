#ifndef LOADER_H
#define LOADER_H
#include "data_type.h"

extern u64 bootloader_info[4];

unsigned long remote_load (unsigned long *base, unsigned long size);
int relocate_process ();
int clear_memory ();

#endif
