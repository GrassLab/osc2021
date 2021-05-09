#ifndef _STRING_H
#define _STRING_H

int strcmp(const char *str1, const char *str2);
int strcpy(char *str1, const char *str2);
int strlen(const char *str);
void * memset ( void * ptr, int value, int num );
int hextoi(char *hexStr);
char *itoa(unsigned int value, char *str, int base);


#endif