#ifndef __SYSCALL_WRAPPER_H_
#define __SYSCALL_WRAPPER_H_

#include <sysreg.h>

#define __MAP0(m, ...)
#define __MAP1(m, t, a, ...) m(t, a)
#define __MAP2(m, t, a, ...) m(t, a), __MAP1(m, __VA_ARGS__)
#define __MAP3(m, t, a, ...) m(t, a), __MAP2(m, __VA_ARGS__)
#define __MAP4(m, t, a, ...) m(t, a), __MAP3(m, __VA_ARGS__)
#define __MAP5(m, t, a, ...) m(t, a), __MAP4(m, __VA_ARGS__)
#define __MAP6(m, t, a, ...) m(t, a), __MAP5(m, __VA_ARGS__)
#define __MAP(n, ...) __MAP##n(__VA_ARGS__)

#define __SC_CAST(t, a) (t) a
#define __SC_LONG(t, a) unsigned long a
#define __SC_DECL(t, a) t a
#define __SC_ARGS(t, a) a

#define SYSCALL_DEFINE0(name, ...) SYSCALL_DEFINEx(0, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE1(name, ...) SYSCALL_DEFINEx(1, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE2(name, ...) SYSCALL_DEFINEx(2, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE3(name, ...) SYSCALL_DEFINEx(3, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE4(name, ...) SYSCALL_DEFINEx(4, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE5(name, ...) SYSCALL_DEFINEx(5, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE6(name, ...) SYSCALL_DEFINEx(6, _##name, __VA_ARGS__)

#define SC_ARM64_REGS_TO_ARGS(x, ...) \
    __MAP(x, __SC_ARGS, , regs->regs[0], , regs->regs[1], , regs->regs[2], , regs->regs[3], , regs->regs[4], , regs->regs[5])

#define SYSCALL(nr, name) \
    [nr] = sys_##name

#define SYSCALL_DEFINEx(x, name, ...)                                   \
    static inline long _do_sys##name(__MAP(x, __SC_LONG, __VA_ARGS__)); \
    static inline long do_sys##name(__MAP(x, __SC_DECL, __VA_ARGS__));  \
    long sys##name(const struct pt_regs *regs)                          \
    {                                                                   \
        return _do_sys##name(SC_ARM64_REGS_TO_ARGS(x, __VA_ARGS__));    \
    }                                                                   \
    static inline long _do_sys##name(__MAP(x, __SC_LONG, __VA_ARGS__))  \
    {                                                                   \
        return do_sys##name(__MAP(x, __SC_CAST, __VA_ARGS__));          \
    }                                                                   \
    static inline long do_sys##name(__MAP(x, __SC_DECL, __VA_ARGS__))

#define SYSCALL_METADATA(name) \
    extern long sys_##name(const struct pt_regs *regs);

#endif