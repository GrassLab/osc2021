## Terms
* bare-metal: develop program without OS
* boot loader: the program that is executed after the BIOS is being booted
* elf: Executable and Linkable Format 
* Peripherals: 周邊設備
* UART: [Universal asynchronous receiver-transmitter](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)，可以把memory-mapped register內的value轉成sequence of high and low voltage and pass it to the computer via TTL-to-serial cable.

## Makefile
當專案有許多檔案，且處理方式複雜，有dependency時，makefile就是好幫手
可以看看此[網站](https://bit.ly/3sOqwON)了解詳情
* COPS: parameters of C compiler
* nostdlib: don't use C std lib. The reason is that C std lib eventually interact with operating system.
* nostartfiles: don't use standard startup files. It would setting an initial stack pointer...... But we'll do it on our own

## Linker Script
describe how the sections in the input object files (???.o) should be mapped into the output file (???.elf). 
Making the size of .bss to be multiple of 8. It allows us to use str to initialize .bss section to zero.

### more explanation on linker script
第一行的SECTION是keywords, 代表開始宣告SECTIONS
"." 在linker script中代表 location counter，會隨著宣告的section而增加。也可以直接assign值給location counter，例如 .=0x80000;
接下來，用個小例子來解釋
>   .rodata : { *(.rodata) }

左方的.rodata代表，定義一個output section，名為rodata(read-only data)。而{}中的內容，則是將input file的rodata放入這個定義的output section。
\*則是wildcard符號，概念很像下指令會使用的\*，就是一個for all的概念
在linker script之中需要紀錄bss.begin和bss.end，為了之後初始化使用
不然linker script只會紀錄section大小

## boot.S
the program being executed after the RPI3 is booted.

### detailed 
* MPIDR_EL1: register that stores the processor ID.
For RPI3 has four cores and we only want to execute the program once, so we check the processor ID of current process. Making sure that only the core with ID equals to zero execute this program.
Then, we ask one processor to initializing bss section.
After initializing the stack pointer, we can execute our kernel program.