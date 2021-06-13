#ifndef __PERIPHERAL_H_
#define __PERIPHERAL_H_

#include <stdint.h>
#include <mmu.h>

#define __REG_TYPE volatile uint64_t
#define __REG uint32_t *

/* Auxiliary peripherals Register Map */
#define AUX             ((__REG_TYPE) PHYS_OFFSET + 0x3f215000)
#define AUXIRQ          ((__REG)(AUX + 0x0))
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
#define GPIO            ((__REG_TYPE) PHYS_OFFSET + 0x3f200000)
#define GPFSEL1         ((__REG) (GPIO + 0x4))
#define GPPUD           ((__REG) (GPIO + 0x94))
#define GPPUDCLK0       ((__REG) (GPIO + 0x98))

/* Interrupt registers */
#define INT                     ((__REG_TYPE) PHYS_OFFSET + 0x3f00b000)
#define INPUT_ENABLE_REGISTER_1 ((__REG) (INT + 0x210))

#define CORE0_TIMER_IRQ_CTRL ((__REG) (PHYS_OFFSET + 0x40000040))
#define CORE0_TIMER_IRQ_SRC ((__REG) (PHYS_OFFSET + 0x40000060))

#endif
