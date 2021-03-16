#include "../include/initrd_my.h"


int mem_compare(void* s1, void* s2, int n){
    unsigned char *a = s1;
    unsigned char *b = s2;
    while(n != 0){
        if(*a != *b){
            return *a - *b;
        }else{
            ++a; ++b;
        }
    }
    return 0;
}


typedef struct {
    char magic[6];
    char ino[8];
    char mode[8];
    char uid[8];
    char gid[8];
    char nlink[8];
    char mtime[8];
    char filesize[8];
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];
    char check[8];
} __attribute__((packed)) ncpio_t;


