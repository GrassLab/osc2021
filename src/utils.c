/*  Place for utils function  */

//inline
void delay(int time) {
    while (time--) {}
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1++ != *s2++) {
            return -1;
        }
    }
    return *s1 == *s2 ? 0 : -1;
}
