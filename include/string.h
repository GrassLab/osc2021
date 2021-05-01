#include "../include/type.h"
#define NULL (void*) 0 

int  strcmp     ( char * s1, char * s2 );
void strset     ( char * s1, int c, int size );
int  strlen     ( char * s );
void itoa       ( int x, char str[], int d);
void itohex_str ( uint64_t d, int size, char * s );
void reverse    ( char *s );
void write_ptr  (void *p);
int  exp        (int num);
int pow         (int base, int exponent);
int ceil        (double x);
