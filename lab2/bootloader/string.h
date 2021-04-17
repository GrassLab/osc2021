#ifndef __STRING__
#define __STRING__

int strcmp(char *str1, char *str2);
int strlcmp(char *str1, char *str2, int len);
char *strstr(char *str1, char *str2, int str_len);
unsigned int str2int(char *str);
unsigned int strl2int(char *str, int len);
unsigned int dec_str2int(char *str);
unsigned long hex_str2long(char *str);
int strlen(char *str);


#endif