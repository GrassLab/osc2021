#include "shell/buffer.h"
#include "shell/cmd.h"
#include "shell/shell.h"

#include "bool.h"
#include "config.h"
#include "cpio.h"
#include "string.h"
#include "uart.h"

#include <stdint.h>

#ifndef RAMFS_ADDR
#define RAMFS_ADDR 0x8000000
#endif

typedef enum AnsiEscType {
  Unknown,
  CursorForward,
  CursorBackward,
} AnsiEscType;

enum KeyboardInput {
  KI_BackSpace = '\b',             // 8
  KI_LineFeed = '\n',              // 10
  KI_CarrageReturn = '\r',         // 13
  KI_Esc = '\e',                   // 27
  KI_ANSI_ESCAPE_SEQ_START = '\e', // 27

  KI_PRINTABLE_START = 32,
  KI_PRINTABLE_END = 126,
  KI_Delete = 127,
};

static AnsiEscType decode_escape_sequence();
static int try_fetch_file(char *filename);
static void shell_update_prompt();

AnsiEscType decode_escape_sequence() {
  char c = uart_getc();
  if (c == '[') {
    // ANSI CSI
    switch (c = uart_getc()) {
    case 'C':
      return CursorForward;
    case 'D':
      return CursorBackward;
    default:
      return Unknown;
    }
  }
  return Unknown;
}

int try_fetch_file(char *filename) {
  unsigned long size;
  uint8_t *file = (uint8_t *)cpioGetFile((void *)RAMFS_ADDR, filename, &size);
  if (file != NULL) {
#ifdef CFG_LOG_SHELL_SEARCH_FILE
    uart_println("  [fetchFile] file addr:%x , size:%d", file, size);
#endif
    for (unsigned long i = 0; i < size; i++) {
      if (file[i] == '\n' && i > 0 && file[i - 1] != '\r') {
        uart_println("");
      } else {
        uart_send(file[i]);
      }
    }
    uart_println("");
    return 0;
  }
  return 1;
}

void shell_init(struct Shell *sh, char *data, uint32_t size) {
  sh->data = data;
  sh->bfr_size = size;
  InputBuffer_init(&sh->bfr, sh->data, sh->bfr_size);
}

void shell_show_prompt(struct Shell *sh) { uart_puts("\r>"); }

void shell_update_prompt(struct Shell *sh) {
  // Must be called after every keystroke user input
  // Assumption: There're at most 1 character change inside the buffer

  // Rebuild buffer
  shell_show_prompt(sh);
  uart_puts(sh->data);

  // User might delete 1 character, here we paint a blank space to "delete it"
  // on the screen
  uart_puts(" ");

  // Restore cursor on the screen
  char *num_space_to_left = itoa(sh->bfr.write_head + 1, 10);
  uart_printf("\r\e[%sC", num_space_to_left);
}

void shell_input_line(struct Shell *sh) {
  enum KeyboardInput c;
  AnsiEscType termCtrl;
  bool flagExit = false;
  sh->bfr.clear(&sh->bfr);

  while (!flagExit) {
    flagExit = false;
    shell_update_prompt(sh);
    switch ((c = uart_getc())) {
    case KI_ANSI_ESCAPE_SEQ_START:
      termCtrl = decode_escape_sequence();
      switch (termCtrl) {
      case CursorForward:
        sh->bfr.cursor_mov_right(&sh->bfr);
        break;
      case CursorBackward:
        sh->bfr.cursor_mov_left(&sh->bfr);
        break;
      case Unknown:
        break;
      }
      break;
    case KI_PRINTABLE_START ... KI_PRINTABLE_END:
      sh->bfr.push(&sh->bfr, c);
      uart_send(c);
      break;
    case KI_BackSpace:
    case KI_Delete:
      sh->bfr.pop(&sh->bfr);
      break;
    case KI_CarrageReturn:
    case KI_LineFeed:
      flagExit = true;
      uart_puts("\r\n");
#ifdef CFG_LOG_SHELL_BUFFER
      uart_println("buffer: '%s'", sh->data);
#endif
      break;
    default:
        // ignore other input
        ;
    }
  }
}

// Process command resides in buffer
void shell_process_command(struct Shell *sh) {
  Cmd *cmd = getCmd(sh->data);
  if (cmd != NULL) {
    cmd->func();
  }
  try_fetch_file(sh->data);
}
