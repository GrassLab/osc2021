# define SCTLR_VALUE_MMU_DISABLED 0

# define HCR_EL2_RW_AARCH64  (1 << 31)
# define HCR_EL2_VALUE       (HCR_EL2_RW_AARCH64)

// SPSR_EL2, Saved Program Status Register (EL2)

# define SPSR_EL2_MASK       (0b1111 << 6)
# define SPSR_EL2_EL1h       (0b0101 << 0)
# define SPSR_EL2_VALUE      (SPSR_EL2_MASK | SPSR_EL2_EL1h)

// SPSR_EL1, Saved Program Status Register (EL1)

# define SPSR_EL1_MASK       (0b0000 << 6)
# define SPSR_EL1_EL0        (0b0000 << 0)
# define SPSR_EL1_VALUE      (SPSR_EL1_MASK | SPSR_EL1_EL0)

