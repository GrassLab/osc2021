#ifndef AUX_H
#define AUX_H

#include "mmio.h"

#define AUX_ENABLES ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IER_REG ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR_REG ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR_REG ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR_REG ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_CNTL_REG ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_BAUD_REG ((volatile unsigned int*)(MMIO_BASE+0x00215068))

#endif
