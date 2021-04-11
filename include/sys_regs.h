#pragma once

// ***************************************
// SCTLR_EL1, System Control Register (EL1), Page 2654 of AArch64-Reference-Manual.
// ***************************************

#define SCTLR_RESERVED              ((3 << 28) | (3 << 22) | (1 << 20) | (1 << 11))
#define SCTLR_EE_LITTLE_ENDIAN      (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN     (0 << 24)
#define SCTLR_I_CACHE_DISABLED      (0 << 12)
#define SCTLR_D_CACHE_DISABLED      (0 << 2)
#define SCTLR_MMU_DISABLED          (0 << 0)
#define SCTLR_MMU_ENABLED           (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED    (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

// ***************************************
// HCR_EL2, Hypervisor Configuration Register (EL2), Page 2487 of AArch64-Reference-Manual.
// https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/hcr_el2
// ***************************************

#define HCR_RW                      (1 << 31) // The Execution state for EL1 is AArch64
#define HCR_EL2_VALUE               HCR_RW

// ***************************************
// SCR_EL2, Secure Configuration Register (EL2),
// ***************************************

#define SCR_RESERVED                (3 << 4)
#define SCR_RW                      (1 << 10)
#define SCR_NS                      (1 << 0)
#define SCR_VALUE                   (SCR_RESERVED | SCR_RW | SCR_NS)

// ***************************************
// SPSR_EL2, Saved Program Status Register (EL2)
// https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/spsr_el2
// ***************************************

#define SPSR_AARCH64                (0 << 4)
#define SPSR_MASK_D                 (1 << 9)
#define SPSR_MASK_A                 (1 << 8)
#define SPSR_MASK_I                 (1 << 7)
#define SPSR_MASK_F                 (1 << 6)
#define SPSR_EL0t                   (0b0000 << 0) // Exception level and selected Stack Pointer. from which level we trap in to el2
#define SPSR_EL1t                   (0b0100 << 0)
#define SPSR_EL1h                   (0b0101 << 0)
#define SPSR_EL2t                   (0b1000 << 0)
#define SPSR_EL2h                   (0b1001 << 0)
#define SPSR_EL2_VALUE              (SPSR_MASK_D | SPSR_MASK_A | SPSR_MASK_I | SPSR_MASK_F | SPSR_EL1h)
#define SPSR_EL1_VALUE              (SPSR_MASK_D | SPSR_MASK_A | SPSR_MASK_F | SPSR_EL0t)
