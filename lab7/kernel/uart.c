#include "gpio.h"
#include "auxilary.h"
#include "uart.h"

char read_buf[MAX_UART_BUFFER];
char write_buf[MAX_UART_BUFFER];
int read_buf_start, read_buf_end;
int write_buf_start, write_buf_end;

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
	
	uart_interrupt_init();
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
    return r; // convert carrige return to newline
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

/*
*	uart_interrupt_init()
*
*   1. enable uart read interrupt
*   2. initial read，write buffer parameter
*   3. enable_uart_interrupt
*
*   AUX_MU_IER
*   https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf  p.12
*
*   0' bit : enable or disable receive interrupt (read)   
*   1' bit : enable or disable transmit interrupt (write)
*
*   0: disable read & write (00)
*   1: enable read, disable write (01)
*   2: enable write, disable read (10)
*   3: enable read & write (11)
*/
void uart_interrupt_init()
{
	*AUX_MU_IER = 1;
	read_buf_start = read_buf_end = 0;
	write_buf_start = write_buf_end = 0;
	enable_uart_interrupt();
}

void enable_uart_interrupt() 
{ 
	*ENABLE_IRQS_1 = AUX_IRQ; 
}

void disable_uart_interrupt() 
{ 
	*DISABLE_IRQS_1 = AUX_IRQ; 
}

void enable_write_interrupt() 
{ 
	*AUX_MU_IER |= 0x2; 
}

void disable_write_interrupt() 
{ 
	*AUX_MU_IER &= ~(0x2); 
}

/*
*	uart_interrupt_handler()
*	
*	1. critical section： before handle，disable interrupt; after handle，enable interrupt
*   2. AUX_MU_IIR https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf p.13
*      0x4 = read, 0x2 = write
*   3. AUX_MU_LSR https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf p.15
*      0x1 = data ready, 0x20 = transmitter empty
*   4. if read, we read byte to reade buffer end inex, and if buffer end index is end ,  index = 0，循環 
*   5. if write
*	   5.1 if write buffer full, disable write interrupt and break;
*      5.2 get char form write buffer start index and send 	
*      2.2 if buffer start = max size,  start = 0，循環 
*/
void uart_interrupt_handler() 
{
	disable_uart_interrupt();

	if (*AUX_MU_IIR & 0x4) // read
	{
		while (*AUX_MU_LSR & 0x1)
		{
			char c = (char)(*AUX_MU_IO);
			read_buf[read_buf_end++] = c;

			if (read_buf_end == MAX_UART_BUFFER) 
				read_buf_end = 0;
		}
	} 
	else if (*AUX_MU_IIR & 0x2) // write
	{
		while (*AUX_MU_LSR & 0x20) 
		{
			if (write_buf_start == write_buf_end) 
			{
				disable_write_interrupt();
				break;
			}
		  
			char c = write_buf[write_buf_start++];
			*AUX_MU_IO = c;
		  
			if (write_buf_start == MAX_UART_BUFFER) 
			    write_buf_start = 0;
		}
	}

	enable_uart_interrupt();
}

/*
*	uart_async_getchar()
*	
*   1. wait until there are new data
*   2. read a char form read buffer start index
*   3. if buffer start index is end, index = 0
*/
char uart_async_getchar() 
{
	// wait until there are new data
	while (read_buf_start == read_buf_end) 
	{
		asm volatile("nop");
	}

	char c = read_buf[read_buf_start++];
	if (read_buf_start == MAX_UART_BUFFER) 
		read_buf_start = 0;
	
	return c;
}

/*
*	uart_async_putstr(s*)
*	
*   1. input char to writer buffer end
*   2. '\n' -> '\r\n'
*   3. if buffer end index is end, index = 0
*   4. enable write intrrupt to transmit 
*/
void uart_async_putstr(char *s) 
{
	for (int i = 0; s[i]; i++) 
	{
		if (s[i] == '\n') 
			write_buf[write_buf_end++] = '\r';
		
		write_buf[write_buf_end++] = s[i];		
		if (write_buf_end == MAX_UART_BUFFER) 
			write_buf_end = 0;
	}
		
	enable_write_interrupt();
}

//================================================================

unsigned long uart_gets(char *buf, int size)
{
    for(int i = 0; i < size; ++i)
	{
        buf[i] = uart_getchar();
        uart_sendchar(buf[i]);
        if(buf[i] == '\n' || buf[i] == '\r')
		{
            buf[i] = '\0';
            return i;
        }
    }
    return size;
}