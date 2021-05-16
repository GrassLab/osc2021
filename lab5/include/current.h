#ifndef __CURRENT_H_
#define __CURRENT_H_

#include <sched.h>

/* in order for linker to work, we need to make it static,
 * since static inline functions are always resolved within the translation unit */
/* https://clang.llvm.org/compatibility.html#inline */

static inline struct task_struct *get_current() {
    return (struct task_struct *)read_sysreg(tpidr_el1);
}
static inline void set_current(struct task_struct *ts) {
    write_sysreg(tpidr_el1, ts);
}

#define current get_current()

#endif