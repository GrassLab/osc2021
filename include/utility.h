#ifndef UTILITY_H
#define UTILITY_H

extern unsigned long long time_counter ();
extern unsigned int time_freq ();
extern unsigned long get_bootloader_start ();
extern unsigned long get_bootloader_end ();
extern unsigned long get__start ();
extern void jump_address (unsigned long address);

#endif
