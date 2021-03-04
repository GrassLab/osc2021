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

void memset(char* a, unsigned int value, unsigned int size) {
    for(unsigned int i=0;i<size;i++) {
        a[i] = value;
    }
}
