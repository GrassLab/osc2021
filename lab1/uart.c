#include "gpio.h"
#include "auxilary.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
	// 1.Set AUXENB register to enable mini UART. Then mini UART register can be accessed.
	// 2.Set AUX_MU_CNTL_REG to 0. Disable transmitter and receiver during configuration.
	// 3.Set AUX_MU_IER_REG to 0. Disable interrupt because currently you don’t need interrupt.
	// 4.Set AUX_MU_LCR_REG to 3. Set the data size to 8 bit.
	// 5.Set AUX_MU_MCR_REG to 0. Don’t need auto flow control.
	/* 6.Set AUX_MU_BAUD to 270. Set baud rate to 115200
	    After booting, the system clock is 250 MHz.
		
						systemx clock freq
		baud rate =  ------------------------
						 8×(AUX_MU_BAUD+1)
    */
	// 7.Set AUX_MU_IIR_REG to 6. No FIFO.
    // 8.Set AUX_MU_CNTL_REG to 3. Enable the transmitter and receiver.
    
    *AUX_ENABLE |=1;       // Enable mini uart (UART1)
    *AUX_MU_CNTL = 0;	   // Disable TX, RX
	*AUX_MU_IER = 0;       // Disable interrupt
    *AUX_MU_LCR = 3;       // Set the data size to 8 bit
    *AUX_MU_MCR = 0;	   // Don't need auto flow control
    *AUX_MU_BAUD = 270;    // set 115200 baud
    *AUX_MU_IIR = 6;       // no FIFO
    
	/* map UART1 to GPIO pins */
	
	//GPIO 14, 15 can be both used for mini UART and PL011 UART. However, mini UART should set 'ALT5' 
	//and PL011 UART should set ALT0 You need to configure 'GPFSELn' register to change alternate function.
	
	//Next, you need to configure pull up/down register to disable GPIO pull up/down. It’s because 
	//these GPIO pins use alternate functions, not basic input-output. 
	//Please refer to the description of 'GPPUD' and 'GPPUDCLKn' registers for a detailed setup.
	
	// 1. Change GPIO 14, 15 to alternate function
	
    register unsigned int r = *GPFSEL1;	// GPFSEL is GPIO Function Select Registers
    r&=~((7<<12)|(7<<15)); // Reset GPIO 14, 15
    r|=(2<<12)|(2<<15);    // set alt5 
    *GPFSEL1 = r;
	
	// 2. Disable GPIO pull up/down (Because these GPIO pins use alternate functions, not basic input-output)
	
	// GPPUD is GPIO Pull-up/down Register
	// GPPUDCLKn is GPIO Pull-up/down Clock Register
    *GPPUD = 0;                                // Set control signal to disable
    r=150; while(r--) { asm volatile("nop"); } // Wait 150 cycles
    *GPPUDCLK0 = (1<<14)|(1<<15);              // Clock the control signal into the GPIO pads
    r=150; while(r--) { asm volatile("nop"); } // Wait 150 cycles
    *GPPUDCLK0 = 0;        					   // Remove the clock
	
    *AUX_MU_CNTL = 3;      // enable Tx, Rx

}

/*
Send a character
1.Check AUX_MU_LSR_REG’s Transmitter empty field.
2.If set, write to AUX_MU_IO_REG
*/
void uart_sendchar(unsigned int c) 
{
    // wait until we can send
    do
	{
		asm volatile("nop");
	}while(!(*AUX_MU_LSR&0x20));
    
    *AUX_MU_IO = c; // write the character to the buffer
}

/*
Receive a character
1. Check AUX_MU_LSR_REG’s data ready field.
2. If set, read from AUX_MU_IO_REG
*/
char uart_getchar() 
{
    char r;
	
    // wait until something is in the buffer
    do
	{
		asm volatile("nop");
	}while(!(*AUX_MU_LSR&0x01));
    
    r = (char)(*AUX_MU_IO);	// read it and return   
    return (r == '\r') ? '\n' : r; // convert carrige return to newline
}

// send a string
void uart_putstr(char *s) 
{
    while(*s) {
        // convert newline to carrige return + newline
        if(*s=='\n')
            uart_sendchar('\r');
        uart_sendchar(*s++);
    }
}
