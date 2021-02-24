# My OSC 2021

## Author

| 學號        | GitHub 帳號  | 姓名    | Email                      |
| ---------- | ----------- | ------- | -------------------------- |
| `A091514`  | `kishow01`  | `林昌德` | kishow01@gmail.com         |

## Toolchain
+ env: Linux ubuntu 18.04

### Cross compiler
``sudo apt install gcc-8-aarch64-linux-gnu``

### QEMU
+ Package Tool
	``sudo apt-get install qemu``
+ Build from source code
	```=
	wget https://download.qemu.org/qemu-5.2.0.tar.xz
	tar xvJf qemu-5.2.0.tar.xz
	cd qemu-5.2.0
	./configure
	make
	make install
	```

#### Troubleshooting
+ ERROR: Cannot find Ninja
	1. Install re2c
		``apt install re2c``
	2. Install ninja
		```=
		git clone git://github.com/ninja-build/ninja.git && cd ninja
		./configure.py --bootstrap
		cp ninja /usr/bin/
		```
+ QEMU build dependencies
	```=
	sudo apt install pkg-config libglib2.0-dev libmount-dev python3 python3-pip python3-dev git libssl-dev libffi-dev build-essential autoconf automake libfreetype6-dev libtheora-dev libtool libvorbis-dev pkg-config texinfo zlib1g-dev unzip cmake yasm libx264-dev libmp3lame-dev libopus-dev libvorbis-dev libxcb1-dev libxcb-shm0-dev libxcb-xfixes0-dev pkg-config texinfo wget zlib1g-dev
	sudo apt-get install -y libpixman-1-dev
	```

## How to build
### Source code to object code
``aarch64-linux-gnu-gcc-8 -c a.S``

### Object code to ELF file
``aarch64-linux-gnu-ld -T linkder.ld -o kernel8.elf a.o``

### ELF file to kernel image
``aarch-linux-gnu-objdump -O binary kernel8.elf kernel8.img``

## How to run

### QEMU
``qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm``

## How to burn it into pi3

**WIP**

## Architecture

**WIP**

## Directory structure

**WIP**