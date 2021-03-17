#ifndef STRING_H
#define STRING_H

int  strcmp     ( char * s1, char * s2 );
void strset     ( char * s1, int c, int size );
int  strlen     ( char * s );
void itoa       ( int x, char str[], int d);
void ftoa       ( float n, char* res, int afterpoint ); 
int atoi (const char * str);
void reverse    ( char *s );
int strncmp(const char *str1, const char *str2, unsigned long n);

#endif