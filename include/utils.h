#ifndef	_BOOT_H
#define	_BOOT_H

extern int get_el ( void );

static inline void delay(unsigned long tick) {
    while (tick--) {
        asm volatile("nop");
    }
}

extern void set_pgd(unsigned long pgd);

// Because printf.c in our printf source code not support 64bit,
// we design a special function to print 64bit value
void print_0x_64bit(void *p);

#endif  /*_BOOT_H */