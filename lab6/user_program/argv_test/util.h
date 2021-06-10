#ifndef UTIL_H
#define UTIL_H

int strlen(char *s); // calculate string length
int strcmp(char *s1, char *s2); // compare two string is equal
int strcmpn(char *s1, char *s2, int n); // compare two string n char is equal
int atoi(char* input); // string to int
unsigned long hex2int(char* hex, int n); // hex string to integer
void strReverse(char *buf, int size);
void unsignedlonglongToStr(unsigned long long num, char *buf);
void unsignedlonglongToStrHex(unsigned long long num, char *buf);
int log2(int input);

#endif