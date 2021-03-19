int strcmp(char *a, char *b) {
    while(*a != '\0') {
        if(*a != *b) return 1;
        a++;
        b++;
    }
    if(*a != *b) return 1;
    return 0;
}