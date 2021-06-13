# define  __ASM_S__
# include "mem_addr.h"
# include "page.h"

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

# define SCTLR_VALUE_MMU_DISABLED 0

# define TCR_CONFIG_REGION_48bit   (((64 - 48) << 0) | ((64 - 48) << 16))
# define TCR_CONFIG_4KB            ((0b00 << 14) |  (0b10 << 30))
# define TCR_CONFIG_DEFAULT        (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)


# define MAIR_DEVICE_nGnRnE        0b00000000
# define MAIR_NORMAL_NOCACHE       0b01000100
# define MAIR_IDX_DEVICE_nGnRnE    0
# define MAIR_IDX_NORMAL_NOCACHE   1
# define MAIR_EL1_VALUE          ((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
                                 (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))

# define BOOT_PGD_ATTR       PD_TABLE
# define BOOT_PUD_DEV_ATTR   (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
# define BOOT_PUD_N_ATTR     (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)
