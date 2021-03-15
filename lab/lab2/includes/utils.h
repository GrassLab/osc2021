#ifndef _UTILS
#define _UTILS

extern void _moveTo(void *, unsigned int);
extern void _branch(void *);

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

int strcmp(char *array_1, char *array_2) {
  int i;
  for (i = 0; array_1[i] != '\0'; i++)
    if (array_1[i] != array_2[i]) return 0;
  if (array_2[i] != '\0') return 0;
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
#endif /*_UTILS */
