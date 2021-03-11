/*  Place for utils function  */

//inline
//Delay about time * 3~4 cycles
void delay(int time) {
    while (time--) {
        asm volatile("nop");
    }
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1++ != *s2++) {
            return -1;
        }
    }
    return *s1 == *s2 ? 0 : -1;
}
