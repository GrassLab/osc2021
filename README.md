# My OSC 2021

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856004`| `calee0219` | `李家安` | sz110010@gmail.com,<br/>calee.cs08@nycu.edu.tw<br/>calee.cs08g@nctu.edu.tw |

## How to build

### Install toolchain

On Fedora

```shell
sudo dnf install -y clang                       # Using clang as compiler
sudo dnf install -y lld                         # Using llvm lld as linker
sudo dnf install -y llvm                        # Using llvm-objcopy as obj file translator
sudo dnf install -y qemu                        # Using qemu as emulator
sudo dnf install -y make                        # Using make as make tools
```

### Build project

``` shell
# All in one
make all    # or make
# Seperated
make obj    # Generate all obj file (.S to .o)
make elf    # Generate ELF file (link obj)
make img    # copy elf to image file, same as make all
# For optional build
ENV=debug make
LANG=rust make # not implement yet
```

## How to run

### Run in qemu and show console

``` shell
qemu-system-aarch64 -M raspi3 -kernel ./kernel8.img -display none -serial null -serial stdio
# -M/-machine: Supported machine, here using Raspberry Pi 3B
# -display: display backend type, none, gtk, ...
```

### Debug on qemu
- Using debug mode when build: `ENV=debug make`
- Run `qemu-system-aarch64 -M raspi3 -kernel ./kernel8.img -display none -serial null -serial stdio -s -S`
- Run `gdb -x gdbcmd.gdb`

## How to burn it into pi3

### Burn into micro SD card
``` shell
dd if=nctuos.img of=/dev/sdc bs=512 # if for input, of for output, bs for block size
cp kernel8.img ${your SD card mount folder} # replace kernel8.elf with your image
```

### UART ping

| UART | RPi |
|:-:|:-:|
| GND | GND / ping 6 |
| RXD | UARTD_TXD / ping 8 / GPIO 14 |
| TXD | UARTD_RXD / ping 10 / GPIO 15 |

### Host tty attach

``` shell
dmesg | grep -i tty                # Using dmesg command to check tty
sudo screen /dev/ttyUSB0 115200    # Attach tty using root permission, or screen may terminate
```

## Architecture

boot.S for sp set, bss clear, then jump to kernel (write by C)

## Directory structure


- Folder architecture
  ``` shell
  /osc2021
  ├── boot
  │   └── boot.S
  ├── c-impl
  │   ├── include
  │   │   ├── mmio.h
  │   │   ├── shell.h
  │   │   └── util.h
  │   ├── lib
  │   │   ├── mmio.c
  │   │   ├── shell.c
  │   │   └── util.c
  │   └── kernel.c
  ├── rust-impl
  ├── linker.lds
  ├── Makefile
  ├── gdbcmd.gdb
  ├── LICENSE
  ├── README.md
  └── REFERENCE.md
  ```

- File usage
  - boot: Files for boot up, runs before kernel
  - c-impl: The kernel source code that implement in C language
    - include: .h header file
    - lib: .c source file, also place object file here after compile
    - kernel.c: main process after OS boot up
  - rust-impl: The kernel source code that implement in rust language
  - *.gdb: gdb helper file
  - linker.lds: linker script
  - Makefile: makefile for os
  - README.md: documentation
  - REFERENCE.md: reference link when doing the labs
