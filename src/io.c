#include "io.h"

#include "exc.h"
#include "mem.h"
#include "timer.h"
#include "uart.h"

// #define LOGGING_DEBUG
#define LOGGING_ERROR
#define LOGGING_PRINT

#define UART_BUF_SIZE 1024
#define UART_RECV_INT 1
#define UART_SEND_INT 2

#define BUF_EMPTY 0
#define BUF_FULL 1
#define BUF_OVERRUN 2
#define BUF_NORM 3

unsigned long nonblock = 0;

typedef struct uart_buf {
  unsigned char *buf;
  unsigned long head;
  unsigned long tail;
  volatile unsigned long flag;
} uart_buf;

uart_buf recv_buf, send_buf;

unsigned char uart_read_buf(uart_buf *buffer) {
  while (buffer->flag == BUF_EMPTY)
    ;
  disable_interrupt();
  unsigned char c = buffer->buf[buffer->head];
  buffer->head = (buffer->head + 1) % UART_BUF_SIZE;
  if (buffer->head == buffer->tail) {
    buffer->flag = BUF_EMPTY;
  } else {
    buffer->flag = BUF_NORM;
  }
  enable_interrupt();
  return c;
}

void uart_write_buf(unsigned char data, uart_buf *buffer) {
  disable_interrupt();
  buffer->buf[buffer->tail] = data;
  buffer->tail = (buffer->tail + 1) % UART_BUF_SIZE;
  if (buffer->flag == BUF_FULL || buffer->flag == BUF_OVERRUN) {
    buffer->head = buffer->tail;
    buffer->flag = BUF_OVERRUN;
  } else if (buffer->head == buffer->tail) {
    buffer->flag = BUF_FULL;
  } else {
    buffer->flag = BUF_NORM;
  }
  enable_interrupt();
}

void init_nonblock_io() {
  disable_interrupt();
  recv_buf.buf = kmalloc(UART_BUF_SIZE);
  recv_buf.head = 0;
  recv_buf.tail = 0;
  recv_buf.flag = BUF_EMPTY;

  send_buf.buf = kmalloc(UART_BUF_SIZE);
  send_buf.head = 0;
  send_buf.tail = 0;
  send_buf.flag = BUF_EMPTY;

  *AUX_MU_IER = UART_RECV_INT;
  *ENABLE_IRQ_1 = (1 << 29);

  nonblock = 1;
  enable_interrupt();
}

// callback
void uart_recv_cb() {
  while ((*AUX_MU_LSR & 0x1) != 0) {
    uart_write_buf((unsigned char)(*AUX_MU_IO), &recv_buf);
  }
  *AUX_MU_IER |= UART_RECV_INT;
}

void uart_send_cb() {
  while ((*AUX_MU_LSR & 0x20) != 0) {
    *AUX_MU_IO = (unsigned int)uart_read_buf(&send_buf);
    if (send_buf.flag == BUF_EMPTY) {
      break;
    }
  }
  if (send_buf.flag != BUF_EMPTY) {
    *AUX_MU_IER |= UART_SEND_INT;
  }
}

void uart_handler() {
  if ((*AUX_MU_IIR & 2) != 0) {
    *AUX_MU_IER &= (~UART_SEND_INT);
    add_task(&uart_send_cb, NULL, 1);
  } else if ((*AUX_MU_IIR & 4) != 0) {
    *AUX_MU_IER &= (~UART_RECV_INT);
    _add_timer(get_timer_cnt() + timer_frq / 2048, &uart_recv_cb, NULL);
  }
}

void putc(char c) {
  if (nonblock) {
    // uart_send_c((unsigned char)(c));
    disable_interrupt();
    if (send_buf.flag== BUF_EMPTY) {
      *AUX_MU_IER |= UART_SEND_INT;
    }
    uart_write_buf((unsigned char)(c), &send_buf);
    enable_interrupt();
  } else {
    uart_send_c((unsigned char)(c));
  }
}

void puts(const char *s) {
  print(s);
  putc('\n');
}

void puts_n(const char *s, unsigned long len) {
  print_n(s, len);
  putc('\n');
}

char getc() {
  if (nonblock) {
    return (char)uart_read_buf(&recv_buf);
    ;
  } else {
    return (char)uart_recv_c();
  }
}

void gets(char *buffer) {
  unsigned long cursor = 0;
  char c;
  while ((c = getc()) != '\n') {
    buffer[cursor++] = c;
  }
  buffer[cursor] = 0;
}

void gets_n(char *buffer, unsigned long len) {
  unsigned long cursor = 0;
  char c;
  while ((c = getc()) != '\n' && cursor < len) {
    buffer[cursor++] = c;
  }
  buffer[cursor] = 0;
}

void log(const char *msg, int flag) {
  if (flag == LOG_DEBUG) {
#ifdef LOGGING_DEBUG
    print(msg);
#endif
  } else if (flag == LOG_ERROR) {
#ifdef LOGGING_ERROR
    print(msg);
#endif
  } else if (flag == LOG_PRINT) {
#ifdef LOGGING_PRINT
    print(msg);
#endif
  }
}

void log_hex(const char *msg, unsigned long num, int flag) {
  if (flag == LOG_DEBUG) {
#ifdef LOGGING_DEBUG
    print(msg);
    putc(' ');
    print_hex(num);
    putc('\n');
#endif
  } else if (flag == LOG_ERROR) {
#ifdef LOGGING_ERROR
    print(msg);
    putc(' ');
    print_hex(num);
    putc('\n');
#endif
  } else if (flag == LOG_PRINT) {
#ifdef LOGGING_PRINT
    print(msg);
    putc(' ');
    print_hex(num);
    putc('\n');
#endif
  }
}

void print(const char *s) {
  while (*s) {
    putc(*s++);
  }
}

void print_n(const char *s, unsigned long len) {
  unsigned long i = 0;
  while (*s && i < len) {
    putc(*s++);
    i++;
  }
}

void print_hex(unsigned long num) {
  print("0x");

  if (num == 0) {
    putc('0');
    return;
  }

  char buf[17];
  for (int i = 15; i >= 0; i--) {
    unsigned char c = (num & 15);
    if (c > 9) {
      buf[i] = 'a' + (c - 10);
    } else {
      buf[i] = '0' + c;
    }
    num = num >> 4;
  }
  buf[16] = 0;

  for (int i = 0; i < 16; i++) {
    if (buf[i] != '0') {
      print(&(buf[i]));
      break;
    }
  }
}
