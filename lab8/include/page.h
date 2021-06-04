
# define PD_TABLE            0b11
# define PD_BLOCK            0b01
# define PD_PAGE             0b11
# define PD_ACCESS           (1 << 10)

# define MAIR_DEVICE_nGnRnE        0b00000000
# define MAIR_NORMAL_NOCACHE       0b01000100
# define MAIR_IDX_DEVICE_nGnRnE    0
# define MAIR_IDX_NORMAL_NOCACHE   1
# define MAIR_EL1_VALUE          ((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
                                 (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))
# define PAGE_SIZE       0x1000
# define PD_LEN          (PAGE_SIZE/8)

# define PGD_ATTR        PD_TABLE
# define PUD0_ATTR       PD_TABLE
# define PUD1_ATTR       (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
# define PME_ATTR        PD_TABLE
# define PTE_DEVICE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)
# define PTE_NORMAL_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)

# ifndef __ASM_S__

# include "typedef.h"

struct pg_t{
  uint64_t pg_data[PD_LEN];
};

extern "C" void kernel_page_setup();
# endif
