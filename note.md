# Lab 2 : Booting

## Requirements
1. UART Bootloader
2. Initial Ramdisk

## Elective
1. Bootloader Self Relocation
2. Devicetree

## UART Bootloader
### Background
載入 kernel 的流程
1. SoC 上的 GPU 載入位於 ROM 的 first-stage bootloader
2. first-stage bootloader 可以辨識 FAT16/32 filesystem，並從 SD 卡上將 second-stage bootloader bootcode.bin 載入到 L2 Cache
3. bootcode.bin 初始化 SDRAM 並載入 start.elf
4. start.elf 藉由 config 將 kernel 與其他資料載入至 memory，並將 CPU 喚醒以開始執行

其他第四步將 kernel 載入的 bootloader 可以替換成其他的 bootloader，例如：ELF loading

### UART
在 lab 1 中，我們若要進行 debug，則要不斷的將位於 host 的 kernel8.img 搬進 SD 卡，再將 SD 卡插到樹莓派上執行。
我們可以藉由將 kernel8.img 替換另一個 bootloader，該 booloader 會透過 UART 載入 kernel8.img 至樹莓派執行，以此做法可以方便 debug。

為了讓 UART 傳送 binary，我們需要一個傳送 raw data 的協定，之後就可以透過 Linux 中的 serial device file，將 kernel 從 host 傳送到樹莓派上。

```
with open('/dev/ttyUSB0', "wb", buffering = 0) as tty:
	tty.write(...)
```

#### Note
在 Qemu 中，可以由 ``qemu-system-aarch64 -serial null -serial pty`` 來建立 pseudo TTY device 以測試我們寫的 bootloader。

### Config Kernel Loading Setting
接下來，我們仍可將 kernel 載入至記憶體位址 0x80000，但是這樣會將 bootloader 蓋掉。因此，我們需要重寫 linker script 來指定其他的 start address。最後，我們將 ``config.txt`` 加入 SD 卡中並由 ``kernel_address=`` 來指定 kernel 載入的位址。

為了將 bootloader 與 kernel 區分清楚，我們也可以由 ``kernel=`` 與 ``arm_64bit=1`` 加入 loading image name。
```
kernel_address=0x60000
kernel=bootloader.img
arm_64bit=1
```

### Implementation

#### Rpi3
```c=
void command_load_image() {
	int32_t is_receive_successful = 0;

	uart_puts("Start Loading Kernel Image...\n");
	uart_puts("Loading Kernel Image at address 0x80000...\n");
	
	char *load_address; = (char *)0x80000;
	
	uart_puts("Please send image from uart now:\n");

	do {
		/* waiting 3000 cycles */
		unsigned int n = 3000;
		while ( n-- ) {
			asm volatile("nop");
		}

		/* send starting signal to receive img from host */
		uart_send(3);
		uart_send(3);
		uart_send(3);
		
		/* read kernel's size */
		int32_t size = 0;

		size  = uart_getc();
		size |= uart_getc() << 8;
		size |= uart_getc() << 16;
		size |= uart_getc() << 24;

		if (size < 64 || size > 1024*1024) {
			// size error
			uart_send('S');
			uart_send('E');            
			
			continue;
		}
		uart_send('O');
		uart_send('K');

		/* start receiving img */
		char *address_counter = load_address;

		while (size --) {
			*address_counter++ = uart_getc();
		}

		/* finish */
		is_receive_successful = 1;

		char output_buffer[30];

		uart_puts("Load kernel at: ");
		itohexstr((uint64_t) load_address, sizeof(char *), output_buffer);
		uart_puts(output_buffer);
		uart_send('\n');
	} while(!is_receive_successful);
}

void command_jump_to_kernel() {
	asm volatile (
		"mov x30, 0x80000;"
		"ret"
	);
}
```

#### Host
```python=
with open('/dev/ttyUSB0', "wb", buffering = 0) as tty:
    tty.write(p32(len(kernel)))
    sleep(1)
    tty.write(kernel)
```


## Initial Ramdisk
在 kernel 初始化後，他會掛載 root filesystem 並執行 init user program。init user program 可以為 script 或是可執行檔，其目標為載入其他的 services 與 drivers。

但是目前我們還沒有實作任何的 filesystem 與 storage driver，因此我們無法用我們的 kernel 從 SD 卡載入任何東西，所以我們只能透過 initial Ramdisk 來載入 user program。

initial Ramdisk 為 bootloader 載入或是 kernel 中嵌入的檔案，它通常為一個壓縮檔且可用來建構 root filesystem。

### New ASCII Format Cpio Archive

Cpio 是一種簡易的壓縮格式，它可用來將 directories 與 files 打包。每一個 directory 與 file 都以帶有 pathname 與 content 的 header 記錄下來。

在 Lab 2，我們要使用 New ASCII Format Cpio format 來建立 cpio archive。可以先建立 ``rootfs`` directory 並將所有需要的檔案塞到裡面，接下來由以下指令壓縮。

```
cd rootfs
find . | cpio -o -H newc > ../initramfs.cpio
cd ..
```

可以由參考[4]，裡面定義了 New ASCII Format Cpio Archive 的結構，此外，我們還需要實作一個 parser 來讀取壓縮檔內的檔案。

在 Lab 2 中，我們僅需在 archive 中放一些純文字檔來測試功能。

### Loading Cpio Archive
#### Qemu

加入 argument ``-initrd <cpio archive>``，Qemu 會載入 cpio archive file 至 0x8000000。(default)

#### Rpi3

將 cpio archive 檔移至 SD 卡中，接者在 ``config.txt`` 中指定 name 與 loading address。

```
initramfs initramfs.cpio 0x20000000
```







## Bootloader Self Relocation



## Devicetree



## References
+ [1] https://grasslab.github.io/NYCU_Operating_System_Capstone/labs/lab2.html#introduction
+ [2] https://blog.nicolasmesa.co/posts/2019/08/booting-your-own-kernel-on-raspberry-pi-via-uart/
+ [3] https://github.com/mrvn/raspbootin
+ [4] https://www.freebsd.org/cgi/man.cgi?query=cpio&sektion=5
+ [5] https://github.com/SEL4PROJ/libcpio

