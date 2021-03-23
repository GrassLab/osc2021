# Lab 1: Hello World
## Requirements
1. Basic Initialization
2. Mini UART
3. Simple Shell

## Elective
1. Reboot

## Basic Initialization
當程式載入時，他會執行以下動作:
1. 所有的 data 都擺在記憶體中正確的位址
2. program counter 被設置到正確的位址
3. 將 bss segment 清空為 0
4. 將 stack pointer 設置成正確的位址

### start.S
參考 [1]，首先，我們必須讓一個 core 來執行我們的程式，**mpidr_el1** 是一個特殊的 system register，它可以表示目前是哪一個 core 在執行該程式。因此，可以讀取 **mpidr_el1** system register，並指讓第 0 個 core 執行程式，其他的 core 就執行無限迴圈。

```
_start:
	mrs x1, mpidr_el1
	and x1, x1, #3
	cbz x1, 2f
// CPU != 0, infinite loop
1:	wfe
	b 1b
// CPU == 0, do something
2: 	...
```

接下來，在呼叫 C 程式前，要將記憶體中 bss segment 清成 0，stack 也要先規劃好。
若 C 程式 return，則該 core 應該與其他 cores 一樣進入無限迴圈。

```
// set stack before our code
2:	ldr x1, =_start
	mov sp, x1
// clear bss segment
	ldr x1, =__bss_start
	ldr w2, =__bss_size
3:	cbz w2, 4f
	str xzr, [x1], #8
	sub w2, w2, #1
	cbnz w2, 3b
// Jump to C program
4:	bl main
	b 1b
``` 

### Linker script
linker script 必須包含 bss section 與 bss size 等等，因此剛剛的 start.S 可以直接使用 ``__bss_start`` 與 ``__bss_size``，除此之外，還需要 .rodate 與 .data 的位址。

```
SECTIONS
{
	.bss (NOLOAD) : {
		. = ALIGN(16);
		__bss_start = .;
		*(.bss .bss.*)
		*(COMMON)
		__bss_end = .;
	}
}
```

上面這段程式定義 bss segment，其中 NOLOAD 表示其內容在運作時不會載入記憶體，ALIGN(16) 表示記憶體位址對齊，接下來的 ``*(.bss .bss.*)`` 與 ``*(COMMON)`` 是載入所有的 .o 檔的 bss 到 bss segment 上。

以下為完整的 linker script
```
SECTIONS
{
    . = 0x80000;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;
```

+ ``.gnu.linkonce``: [2] 這些 section 是 g++ 使用的，g++ 會在自己的 section 擴展 template。這些 symbol 會被定義為 weak 且允許多重定義，而 linker 會將一個 section 納入，其餘丟皆棄
+ ``KEEP()``: [3] 當有開啟 link-time garbage collection 的時候，有開 KEEP 的區域不會被回收
+ ``PROVIDE()``: [4] 若程式沒有提供這個 symbol (也就是標示 ``_data = .`` )，才會使用這裡提供的 symbol。
+ ``*(COMMON)``: 表示所有未被初始化的資料

## Mini UART
以下內容參考 [5] 與 [6]
RPI3 透過 MMIO 存取周遭裝置，當 CPU 對特定的位址讀寫時，它可以對周遭裝置進行讀寫。

**NOTE: MMU 會將 physical address 0x3f000000 對映到 bus address 0x7e000000，而大部分的 reference 中的都是 bus address。**

### GPIO
在使用 UART 前，需要將 gpio 調成對應的 mode。可以用 gpio 14, 15 作為 UART 或是 PL011 UART。但是，mini UART 應該設定成 ALT5 而 PL011 UART 應該設定成 ALT0，可以由 GPFSELn register 來改成對應的 mode。

接下來，因為 GPIO pins 並不是做為 basic input-output，因此要由 pull up/down register 來停用 GPIO pull up/down。

#### 先設定一些 Address (參考 [5])
> Physical addresses range from 0x3F000000 to 0x3FFFFFFF for peripherals.

![](https://i.imgur.com/7orFeC1.png)

```c=
#define MMIO_BASE       0x3F000000

#define GPFSEL0         ((volatile unsigned int*)(MMIO_BASE+0x00200000))
#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPFSEL2         ((volatile unsigned int*)(MMIO_BASE+0x00200008))
#define GPFSEL3         ((volatile unsigned int*)(MMIO_BASE+0x0020000C))
#define GPFSEL4         ((volatile unsigned int*)(MMIO_BASE+0x00200010))
#define GPFSEL5         ((volatile unsigned int*)(MMIO_BASE+0x00200014))
#define GPSET0          ((volatile unsigned int*)(MMIO_BASE+0x0020001C))
#define GPSET1          ((volatile unsigned int*)(MMIO_BASE+0x00200020))
#define GPCLR0          ((volatile unsigned int*)(MMIO_BASE+0x00200028))
#define GPLEV0          ((volatile unsigned int*)(MMIO_BASE+0x00200034))
#define GPLEV1          ((volatile unsigned int*)(MMIO_BASE+0x00200038))
#define GPEDS0          ((volatile unsigned int*)(MMIO_BASE+0x00200040))
#define GPEDS1          ((volatile unsigned int*)(MMIO_BASE+0x00200044))
#define GPHEN0          ((volatile unsigned int*)(MMIO_BASE+0x00200064))
#define GPHEN1          ((volatile unsigned int*)(MMIO_BASE+0x00200068))
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098))
#define GPPUDCLK1       ((volatile unsigned int*)(MMIO_BASE+0x0020009C))
```

#### 設定 mini UART registers (參考 [5])

![](https://i.imgur.com/vzjvaIo.png)

```c=
/* Auxilary mini UART registers */
#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))
```

### Mini UART
#### Initialization
1. Set AUXENB register to enable mini UART. Then mini UART register can be accessed.
2. Set AUX_MU_CNTL_REG to 0. Disable transmitter and receiver during configuration.
3. Set AUX_MU_IER_REG to 0. Disable interrupt because currently you don’t need interrupt.
4. Set AUX_MU_LCR_REG to 3. Set the data size to 8 bit.
5. Set AUX_MU_MCR_REG to 0. Don’t need auto flow control.
6. Set AUX_MU_BAUD to 270. Set baud rate to 115200
	+ Booting 後，system clock 為 250 MHz
		$baud\ rate=\frac{systemx\ clock\ freq}{8*(AUX\_MU\_MAUD+1)}$
7. Set AUX_MU_IIR_REG to 6. No FIFO.
8. Set AUX_MU_CNTL_REG to 3. Enable the transmitter and receiver.

```c=
/* Set baud rate and characteristics (115200 8N1) and map to GPIO */
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *AUX_ENABLE |=1;       // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_LCR = 3;       // 8 bits
    *AUX_MU_MCR = 0;
    *AUX_MU_BAUD = 270;    // 115200 baud
    *AUX_MU_IIR = 0xc6;    // disable interrupts

    /* map UART1 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15
    r|=(2<<12)|(2<<15);    // alt5
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r=150; 
    while(r--) { 
        asm volatile("nop"); 
    }
	
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; 
    while(r--) {
        asm volatile("nop"); 
    }
	
    *GPPUDCLK0 = 0;        // flush GPIO setup
	
    *AUX_MU_CNTL = 3;      // enable Tx, Rx
}
```

#### Read data
1. Check AUX_MU_LSR_REG’s data ready field.
2. If set, read from AUX_MU_IO_REG

```c=
/* Receive a character */
char uart_getc() {
    char r;
    /* wait until something is in the buffer */
	
    do { 
        asm volatile("nop"); 
    } while(!(*AUX_MU_LSR&0x01));
	
    /* read it and return */
    r=(char)(*AUX_MU_IO);
	
    /* convert carrige return to newline */
    return r=='\r'?'\n':r;
}
```


#### Write data
1. Check AUX_MU_LSR_REG’s Transmitter empty field.
2. If set, write to AUX_MU_IO_REG

```c=
/* Send a character */
void uart_send(unsigned int c) {
    /* wait until we can send */
    do { 
    	asm volatile("nop"); 
    } while(!(*AUX_MU_LSR&0x20));
    
    /* write the character to the buffer */
    *AUX_MU_IO=c;
}
```

#### Write string
```c=
void uart_puts(char *s) {
    while(*s) {
        uart_send(*s++);
    }
}
```

#### Interrupt
+ AUX_MU_IER_REG: enable tx/rx interrupt
+ AUX_MU_IIR_REG: check interrupt cause
+ Interrupt enable register1(page 116 of manual): set 29 bit to enable. (AUX interrupt enable)

**NOTE: qemu 不會將 UART1 導向至 terminal，因此要使用 ``-serial null -serial stdio``**

## Simple Shell
...

## References
+ [1] https://github.com/bztsrc/raspi3-tutorial/tree/master/02_multicorec
+ [2] https://gcc.gnu.org/legacy-ml/gcc/2003-09/msg00984.html
+ [3] https://blog.louie.lu/2016/11/06/10%E5%88%86%E9%90%98%E8%AE%80%E6%87%82-linker-scripts/
+ [4] http://wen00072.github.io/blog/2014/03/14/study-on-the-linker-script/#assign-prov
+ [5] https://github.com/bztsrc/raspi3-tutorial/tree/master/03_uart1
+ [6] https://grasslab.github.io/NYCU_Operating_System_Capstone/hardware/uart.html
