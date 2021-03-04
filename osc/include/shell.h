#pragma once

#define MX_CMD_BFRSIZE 64

typedef enum AnsiEscType {
  Unknown,
  CursorForward,
  CursorBackward,
} AnsiEscType;

enum KeyboardInput {
  KI_BackSpace = '\b',             // 18
  KI_LineFeed = '\n',              // 10
  KI_CarrageReturn = '\r',         // 13
  KI_Esc = '\e',                   // 27
  KI_ANSI_ESCAPE_SEQ_START = '\e', // 27

  KI_PRINTABLE_START = 32,
  KI_PRINTABLE_END = 126,
  KI_Delete = 127,
};

void shellPrintPrompt();
void shellInputLine();
void shellProcessCommand();
