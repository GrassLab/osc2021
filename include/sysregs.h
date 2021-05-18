#ifndef _SYSREGS_H
#define _SYSREGS_H

// ***************************************
// SCTLR_EL1, System Control Register (EL1), Page 2654 of AArch64-Reference-Manual.
// ***************************************

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED	    (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

// ***************************************
// HCR_EL2, Hypervisor Configuration Register (EL2), Page 2487 of AArch64-Reference-Manual.
// ***************************************

#define HCR_RW	    		(1 << 31)
#define HCR_EL2_VALUE		HCR_RW


// ***************************************
// SPSR_EL2, Saved Program Status Register (EL2) Page 389 of AArch64-Reference-Manual.
// ***************************************

#define SPSR_MASK_ALL 		(7 << 6)
//#define SPSR_EL1h			(5 << 0)
#define SPSR_VALUE			(SPSR_MASK_ALL | SPSR_EL1h)

#define SPSR_MASK_D         (1 << 9)
#define SPSR_MASK_A         (1 << 8)
#define SPSR_MASK_I         (1 << 7)
#define SPSR_MASK_F         (1 << 6)
#define SPSR_EL0t           (0b0000 << 0) // Set exception level and selected Stack Pointer
#define SPSR_EL1t           (0b0100 << 0)
#define SPSR_EL1h           (0b0101 << 0)	
#define SPSR_EL2t           (0b1000 << 0)	
#define SPSR_EL2h           (0b1001 << 0)	
#define SPSR_EL2_VALUE      (SPSR_MASK_A | SPSR_MASK_I | SPSR_MASK_F | SPSR_EL1h)
#define SPSR_EL1_VALUE      (SPSR_EL0t)   // unmake intterput and use el0 sp  


// CPACR_EL1, Architectural Feature Access Control Register
#define CPACR_EL1_FPEN      (0b11 << 20)
#define CPACR_EL1_VALUE     (CPACR_EL1_FPEN)

// ***************************************
// ESR_EL1, Exception Syndrome Register (EL1). Page 2431 of AArch64-Reference-Manual.
// ***************************************

#define ESR_ELx_EC_SHIFT		26
#define ESR_ELx_EC_SVC64		0x15

#endif