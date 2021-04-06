/* Delay about time * 3~4 cycles */
void delay(int time) {
    while (time--) {
        asm volatile("nop");
    }
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1++ != *s2++)
            return -1;
    }
    return *s1 == *s2 ? 0 : -1;
}

int char_2_int(char c) {
    switch (c) {
      case '0':
          return 0;
      case '1':
          return 1;
      case '2':
          return 2;
      case '3':
          return 3;
      case '4':
          return 4;
      case '5':
          return 5;
      case '6':
          return 6;
      case '7':
          return 7;
      case '8':
          return 8;
      case '9':
          return 9;
      case 'A':
          return 10;
      case 'B':
          return 11;
      case 'C':
          return 12;
      case 'D':
          return 13;
      case 'E':
          return 14;
      case 'F':
          return 15;
    }
    return 0;
}

unsigned int str_2_int(const char *s) {
    unsigned int pos = 0;
    unsigned int num = 0;
    while(s[pos++]) {
        num *= 10;
        num += char_2_int(s[pos - 1]);
    }
    return num;
}

unsigned long pow(unsigned int base,
                  unsigned int order) {
    unsigned long sum = 1;
    for (int i = 0; i < order; i++) {
        sum *= base;
    }
    return sum;
}

void memset(void *addr,
            unsigned long size,
            char data) {
    for (unsigned long i = 0; i < size; i++) {
        *((char*)addr + i) = data;
    }
}
