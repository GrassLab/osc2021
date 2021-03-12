#ifndef LOADER_H
#define LOADER_H

unsigned long remote_load (unsigned long *base, unsigned long size);
int relocate_process ();
int clear_memory ();

#endif
