#pragma once

#define MX_CMD_BFRSIZE 64

enum InputChar {
  BackSpace = 8,
  LineFeed = 10,
  CarrageReturn = 13,

  PRINTABLE_START = 32,
  PRINTABLE_END = 126,
  Delete = 127
};

void shellInputLine();
void shellProcessCommand();
