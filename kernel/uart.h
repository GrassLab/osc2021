#ifndef _LCD_UART_H_
#define _LCD_UART_H_

/* Auxilary mini UART registers */
/* https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf p.8 */

#define PM_RSTC         ((volatile unsigned int*)0x3F10001C)
#define PM_WDOG         ((volatile unsigned int*)0x3F100024)
#define PM_PASSWORD     (0x5a000000)

void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);

#endif