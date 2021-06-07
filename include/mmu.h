#ifndef _MMU_H
#define _MMU_H

#define MM_TYPE_PAGE_TABLE      0b11
#define MM_TYPE_PAGE            0b11
#define MM_TYPE_BLOCK           0b01
#define MM_ACCESS               (0x1 << 10)
#define MM_ACCESS_PERMISSION    (0x01 << 6) 

/*
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *                  n   MAIR
 *   DEVICE_nGnRnE  000 00000000
 *   NORMAL_NOCACHE 001 01000100
 */
#define MAIR_DEVICE_nGnRnE      0b00000000
#define MAIR_NORMAL_NOCACHE     0b01000100
#define MAIR_IDX_DEVICE_nGnRnE  0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define MMU_FLAGS               (MM_TYPE_BLOCK | (MAIR_IDX_NORMAL_NOCACHE << 2) | MM_ACCESS)	
#define MMU_DEVICE_FLAGS        (MM_TYPE_BLOCK | (MAIR_IDX_DEVICE_nGnRnE << 2) | MM_ACCESS)	
#define MMU_PTE_FLAGS           (MM_TYPE_PAGE | (MAIR_IDX_NORMAL_NOCACHE << 2) | MM_ACCESS | MM_ACCESS_PERMISSION)	

#define MAIR_VALUE              ((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))

/**
 * For TCR_EL1 system register3
 **/
#define TCR_T0SZ			    (64 - 48)                   // T0SZ field of tcr_el1 register is used for Lower VA subrange 
#define TCR_T1SZ			    ((64 - 48) << 16)           // T1SZ field of tcr_el1 register is used for Lower VA subrange Upper VA subrange
#define TCR_TG0_4K              (0b00 << 14)                // Page size(granule size) for user space (or for TG0 is more accurate)
#define TCR_TG1_4K              (0b10 << 30)                // Page size(granule size) for kernel (or for TG1 is more accurate)
#define TCR_CONFIG_REGION_48bit (TCR_T0SZ | TCR_T1SZ)
#define TCR_CONFIG_4KB          (TCR_TG0_4K |  TCR_TG1_4K)
#define TCR_VALUE			    (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#endif