# My OSC 2021

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856004`| `calee0219` | `李家安` | sz110010@gmail.com,calee.cs08@nycu.edu.tw |

## How to build

### Install toolchain

On Fedora

```shell
sudo dnf install -y clang                       # Using clang as compiler
sudo dnf install -y gcc-aarch64-linux-gnu       # Using gcc ld as linker
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
```

## How to run

### Run in qemu and show the asm

``` shell
qemu-system-aarch64 -M raspi3 -kernel ./kernel8.img -display none -d in_asm
# -M/-machine: Supported machine, here using Raspberry Pi 3B
# -display: display backend type, none, gtk, ...
# -d: debugging, in_asm for Show generated host assembly code for each compiled Translation-Block (TB)
```

## How to burn it into pi3

### Burn into micro SD card
``` shell
dd if=nctuos.img of=/dev/sdc bs=512 # if for input, of for output, bs for block size
```

### UART ping

| UART | RPi |
|:-:|:-:|
| GND | GND / ping 6 |
| RXD | UARTD_TXD / ping 8 / GPIO 14 |
| TXD | UARTD_RXD / ping 10 / GPIO 15 |

### Host tty attach

``` shell
dmesg | grep -i tty         # Using dmesg command to check tty
sudo screen /dev/ttyUSB0    # Attach tty using root permission, or screen may terminate
```

## Architecture

**WIP**

## Directory structure

**WIP**
