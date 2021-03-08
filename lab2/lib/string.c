#include <types.h>
#include "string.h"

int strlen(const char *str){
  int len;

  for(len = 0; *str; str++) len++;
  return len;

}

int strncmp(const char *str1, const char *str2, uint32_t n){
    while ((*str1 || *str2) && n > 0){
        if (*str1 == *str2){
            str1++;
            str2++;
            n--;
        }
        else return *str1 - *str2;
    }
    return 0;
}

char *strncpy(char *dest, const char *src, uint32_t n){
    char *addr = dest;

    while (*src && n > 0){
        *dest++ = *src++;
        n--;
    }
    *dest = '\0';
    return addr;
}
void memset(char* str, char c) {
  for(int i = 0; i < strlen(str); i++) {
    str[i] = c;
  }
}
