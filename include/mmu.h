#ifndef _MMU_H_
#define  _MMU_H_

#define TTBR0_EL1_REGION_BIT    48
#define TTBR1_EL1_REGION_BIT    48
#define TTBR0_EL1_GRANULE       0b00 /* 4KB */
#define TTBR1_EL1_GRANULE       0b10 /* 4KB */

#define TCR_EL1_T0SZ            ((64 - TTBR0_EL1_REGION_BIT) << 0)
#define TCR_EL1_T1SZ            ((64 - TTBR1_EL1_REGION_BIT) << 16)
#define TCR_EL1_TG0             (TTBR0_EL1_GRANULE << 14)
#define TCR_EL1_TG1             (TTBR1_EL1_GRANULE << 30)
#define TCR_EL1_VALUE           (TCR_EL1_T0SZ | TCR_EL1_T1SZ | TCR_EL1_TG0 | TCR_EL1_TG1)

#define MAIR_DEVICE_nGnRnE      0b00000000
#define MAIR_NORMAL_NOCACHE     0b01000100
#define MAIR_IDX_DEVICE_nGnRnE  0
#define MAIR_IDX_NORMAL_NOCACHE 1
#define MAIR_EL1_VALUE          ((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))

#define PGD_ADDR				        0x0
#define PUD_ADDR				        0x1000
#define PMD_ADDR                0x2000
#define PD_TABLE                0b11
#define PD_BLOCK                0b01
#define PD_PAGE                 0b11
#define PD_ACCESS               (1 << 10)
#define PD_EL0_ACCESS           (1 << 6)
#define PGD_ATTR                PD_TABLE
#define PUD_ATTR                PD_TABLE
#define PMD_ATTR                PD_TABLE
#define PUD_NORMAL_ATTR         (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)
#define PUD_DEVICE_ATTR         (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define PMD_NORMAL_ATTR         (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)
#define PMD_DEVICE_ATTR         (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define PTE_NORMAL_ATTR         (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)
#define PTE_DEVICE_ATTR         (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)

#define KERNEL_MAPPING          0xffff000000000000
#define PA_MASK                 0xffffffffffff

#endif
