#ifndef _STRING_H
#define _STRING_H

int strcmp(char *str1, char *str2);
int strcpy(char *str1, char *str2);
int strlen(char *str);
void * memset ( void * ptr, int value, int num );
int hextoi(char *hexStr);
char *itoa(unsigned int value, char *str, int base);


#endif