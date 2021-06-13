#ifndef __SYSREG_H_
#define __SYSREG_H_

#define read_sysreg(r) ({                       \
    unsigned long __val;                        \
    asm volatile("mrs %0, " #r : "=r" (__val)); \
    __val;                                      \
})

#define write_sysreg(r, __val) ({                  \
	asm volatile("msr " #r ", %0" :: "r" (__val)); \
})

#define ESR_ELx_EC(esr)	((esr & 0xFC000000) >> 26)
#define ESR_ELx_ISS(esr)	(esr & 0x03FFFFFF)

#define ESR_ELx_EC_SVC64 0b010101
#define ESR_ELx_EC_DABT_LOW 0b100100
#define ESR_ELx_EC_IABT_LOW 0b100000
#define ESR_ELx_EC_BRK_LOW 0b110000

struct pt_regs {
    unsigned long regs[31];
    unsigned long sp;
    unsigned long pc;
    unsigned long pstate;
};

#endif