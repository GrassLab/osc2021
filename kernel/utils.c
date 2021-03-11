int atoi(char* s) {
    int i = 0;
    int num = 0;
    int negative = 0;

    while(s[i] != '\0') {
        if(i == 0 && s[i] == '-') {
            negative = 1;
        }
        else {
            int digit = s[i] - '0';
            num = num*10 + digit;
        }
        i++;
    }
    
    if(negative) return num *= -1;
    return num;
}


void itoa(int num, char *s) {
    int i = 0;
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
