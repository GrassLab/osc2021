#ifndef __ENTRY_H__
#define __ENTRY_H__

#define USER_PROGRAM_ADDR 0x30000000
extern void _load_user_program(void*, void*);
extern int get_el(void);

#endif