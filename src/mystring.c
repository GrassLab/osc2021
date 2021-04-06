int strcmp(char* a, char* b) {
    int flag = 0;
    while(flag==0) {
        if(*a > *b) {
            return 1;
        }
        else if(*a < *b) {
            return -1;
        }

        if(*a==0) {
            break;
        }
        a++;
        b++;
    }
    return flag;
}

int strncmp(char* a, char* b, unsigned long n) {
    for(int i=0;i<n;i++) {
        if(a[i] != b[i]) {
            return a[i] - b[i];
        }
        if(a[i] == 0) {
            return 0;
        }
    }
    return 0;
}

void *memcpy(void *dest, void *src, unsigned long n) {
    char *d = dest;
    char *s = src;
    while(n--) {
        *d++ = *s++;
    }
    return dest;
}

void memset(char* a, unsigned int value, unsigned int size) {
    for(unsigned int i=0;i<size;i++) {
        a[i] = value;
    }
}
