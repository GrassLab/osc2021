#ifndef STRING_H
#define STRING_H

int  strcmp     (const char * s1, const char * s2 );
void strset     ( char * s1, int c, int size );
int  strlen     (const char * s );
void itoa       ( int x, char str[], int d);
void ftoa       ( float n, char* res, int afterpoint ); 
int atoi (const char * str);
void reverse    ( char *s );
int strncmp(const char *str1, const char *str2, unsigned long n);
int strcpy(char *dest, const char *src);

#endif