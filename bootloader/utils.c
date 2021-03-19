int atoi(char* s, int base) {
    int i = 0;
    int num = 0;
    int negative = 0;

    while(s[i] != '\0') {
        if(i == 0 && s[i] == '-') {
            negative = 1;
        }
        if(base == 10) {
            int digit = s[i] - '0';
            num = num*10 + digit;
        }
        if(base == 16) {
            int digit = s[i] >= 'A' ? s[i] - 'A'+ 10 : s[i] - '0';
            num = num*16 + digit;
        }
        i++;
    }
    
    if(negative) return num *= -1;
    return num;
}


void itoa(int num, char *s) {
    int i = 0;
    // handel the special case
    if(num == 0) {
        s[i++] = '0';
    }

    while(num != 0) {
        int digit = num % 10;
        s[i++] = digit + '0';
        num /= 10;
    }
    s[i] = '\0';
    
    // reverse s
    for(int j = i-1; j >= i/2; j--) {
        char temp = s[j];
        s[j] = s[i-1-j];
        s[i-1-j] = temp;
    }
}

unsigned char *subStr(unsigned char *s, int length) {
    static unsigned char cRes[1024];
    unsigned char *pRes = cRes;
    int i;
    for(i = 0; i < length; i++) {
        *pRes = s[i];
        pRes++;
    }
    *pRes = '\0';
    return cRes;
}