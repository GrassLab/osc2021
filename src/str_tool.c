int strcmp(char *s1, char *s2){
    while(*s1!='\0' && *s2!='\0' && *s1==*s2){
        s1 += 1;
        s2 += 1;
    }
    return *s1 - *s2;
}