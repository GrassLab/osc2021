#ifndef _UTILS
#define _UTILS
#define buff_size 64
#define GB 0x40000000
#define MB 0x100000
#define KB 0x400
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

extern void _moveTo(void *, unsigned int);
extern void _branch(void *);
extern void _run_el0(void *, void *);
extern uint64_t _get_el();
extern void _core_timer_enable(uint64_t);
extern void _core_timer_disable();

char *itoa(int num, char *str) {
  int i = 0;
  char buff[64];
  for (; num; i++, num /= 10) buff[i] = '0' + num % 10;
  str[i] = '\0';
  for (int j = 0; j < i; j++) str[j] = buff[i - j - 1];
  return str;
}

int atoi_size(char *str, unsigned int size) {
  int num = 0;
  for (int i = 0; i < size; i++, num *= 10) num += (int)(str[i] - '0');
  return num / 10;
}
int atoi(char *str) {
  int num = 0;
  for (int i = 0; str[i] != '\0'; i++, num *= 10) num += (int)(str[i] - '0');
  return num / 10;
}

int atoHex_size(char *str, unsigned int size) {
  int num = 0;
  for (int i = 0; i < size; i++, num *= 16) {
    if (str[i] > 'F') str[i] = str[i] - 'a' + 'A';
    if ('A' <= str[i] && str[i] <= 'F')
      num += 10 + (int)(str[i] - 'A');
    else
      num += (int)(str[i] - '0');
  }
  return num / 16;
}

int atoHex(char *str) {
  int num = 0;
  for (int i = 0; str[i] != '\0'; i++, num *= 16) {
    if (str[i] > 'F') str[i] = str[i] - 'a' + 'A';
    if ('A' <= str[i] && str[i] <= 'F')
      num += 10 + (int)(str[i] - 'A');
    else
      num += (int)(str[i] - '0');
  }
  return num / 16;
}
void mem_reset(char *buff, int size) {
  for (int i = 0; i < size; i++) buff[i] = '\0';
}
int strlen(char *str) {
  int i = 0;
  while (str[i] != '\0') i++;
  return i;
}
int strcmp(char *array_1, char *array_2) {
  int i;
  for (i = 0; array_1[i] != '\0'; i++)
    if (array_1[i] != array_2[i]) return 0;
  if (array_2[i] != '\0') return 0;
  return 1;
}
int strcmpLen(char *array_1, char *array_2, int len) {
  int i;
  for (i = 0; i < len; i++)
    if (array_1[i] != array_2[i]) return 0;
  return 1;
}
void readline(char *buff) {
  char c;
  mem_reset(buff, buff_size);
  for (int i = 0; (c = uart_getc()); i++) {
    if (c == '\r') {
      uart_puts("\r\n");
      break;
    }
    /* backspace handler */
    else if (c == '\b') {
      buff[--i] = 0;
      if (i >= 0) uart_puts("\b \b");
      if (i-- < 0) i = -1;
      continue;
    }
    buff[i] = c;
    uart_send(c);
  }
}
void get_cmd(char *buff) {
  readline(buff);
  int i;
  for (i = 0; buff[i] != '\0'; i++)
    if (buff[i] == ' ') buff[i] = '\0';
  buff[i + 1] = '\0';
}

void print_int(int i) {
  char buff[buff_size];
  uart_puts(itoa(i, buff));
  uart_puts("\r\n");
}

unsigned long long int to_int(char *buff, unsigned int size) {
  unsigned long long int result = 0;
  for (int i = 0; i < size - 1; i++, result <<= 8) result += buff[i];
  result += buff[size - 1];
  return result;
}

void print_char(char *buff, int size) {
  char str[buff_size];
  mem_reset(str, buff_size);
  for (int i = 0; i < size; i++) str[i] = buff[i];
  uart_puts(str);
}

unsigned long int big2little(unsigned long int addr) {
  return ((addr >> 24) & 0x000000FF) | ((addr << 8) & 0x00FF0000) |
         ((addr >> 8) & 0x0000FF00) | ((addr << 24) & 0xFF000000);
}

void print_hc(char x) {
  for (int c = 4; c >= 0; c -= 4) {
    int n = (x >> c) & 0xF;
    n += n > 9 ? 'A' - 10 : '0';
    uart_send(n);
  }
};

void print_h(unsigned long int x) {
  uart_puts("0x");
  for (int c = 28; c >= 0; c -= 4) {
    int n = (x >> c) & 0xF;
    n += n > 9 ? 'A' - 10 : '0';
    uart_send(n);
  }
  uart_puts("\r\n");
};
int get_int32(unsigned int **addr) {
  int result = **addr;
  (*addr)++;
  return result;
}
int get_int64(unsigned long long int **addr) {
  unsigned long long int result = **addr;
  (*addr)++;
  return result;
}
int get_char8(char **addr) {
  int result = **addr;
  (*addr)++;
  return result;
}
int align(int reminder, int base) { return (base - (reminder % base)) % base; }
int find(char *sub, char *str) {
  int i, len = strlen(sub);
  if (len == 0) return strlen(str) + 1;
  for (i = 0; str[i] != '\0'; i++)
    if (strcmpLen(sub, &str[i], len)) return i;
  return -1;
}
void swap(void **a, void **b) {
  void *tmp = *a;
  *a = *b;
  *b = tmp;
}
// for buddy system
int size2Index(int size) {
  int i = 0;
  while (size >>= 1) ++i;
  return i;
}
int abs(int a) {
  if (a < 0) return -a;
  return a;
}
#endif /*_UTILS */
