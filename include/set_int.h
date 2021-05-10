#ifndef _SET_INT_H_
#define  _SET_INT_H_

#include "type.h"

void from_el2_to_el1(void);
void from_el1_to_el0(int, char *const[], uint64_t, uint64_t);
void leave_exception(void);

#endif
