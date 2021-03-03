#ifndef __REG_H_
#define __REG_H_

#include <stdint.h>
#define __REG_TYPE volatile uint32_t
#define __REG __REG_TYPE *

#define INPUT_ENABLE_REGISTER_1 ((__REG) 0x210)

/* Auxiliary peripherals Register Map */
#define AUX             ((__REG_TYPE) 0x3f215000)
#define AUXENB          ((__REG)(AUX + 0x4))
#define AUX_MU_IO_REG   ((__REG)(AUX + 0x40))
#define AUX_MU_IER_REG  ((__REG)(AUX + 0x44))
#define AUX_MU_IIR_REG  ((__REG)(AUX + 0x48))
#define AUX_MU_MCR_REG  ((__REG)(AUX + 0x50))
#define AUX_MU_LSR_REG  ((__REG)(AUX + 0x54))
#define AUX_MU_CNTL_REG ((__REG)(AUX + 0x60))
#define AUX_MU_BAUD     ((__REG)(AUX + 0x68))
#define AUX_MU_LCR_REG  ((__REG)(AUX + 0x7c))

/* GPIO Memory Map */
#define GPIO            ((__REG_TYPE) 0x3f200000)
#define GPFSEL1         ((__REG) (GPIO + 0x4))

#endif
