#ifndef	_BOOT_H
#define	_BOOT_H

extern int get_el ( void );

static inline void delay(unsigned long tick) {
    while (tick--) {
        asm volatile("nop");
    }
}

#endif  /*_BOOT_H */