#ifndef __UTIL__
#define __UTIL__

#define NULL 0

unsigned int big_to_little_32(unsigned int b_num);
unsigned long big_to_little_64(unsigned long b_num);
unsigned long align(unsigned long num, int size);
void set_time();
unsigned long get_time();
void print_register();

#endif