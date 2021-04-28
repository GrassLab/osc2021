#ifndef _LCD_EXCEPTION_H_
#define _LCD_EXCEPTION_H_

#ifndef __ASSEMBLER__

void exception_entry();
unsigned long get_current_exception_level();

#endif
#endif