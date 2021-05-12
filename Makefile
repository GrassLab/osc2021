CC       := aarch64-linux-gnu-gcc
CXX      := aarch64-linux-gnu-g++
LD       := aarch64-linux-gnu-ld
OBJCOPY  := aarch64-linux-gnu-objcopy
CCFLAGS  := -std=gnu17
CXXFLAGS := -ffreestanding -nostdinc -nostdlib -nostdinc++ -nostartfiles -g -std=c++17 -MMD -I$(CURDIR)/include
export
PROGS     := $(patsubst %,initramfs/%,$(filter-out include lib,$(patsubst program/%,%,$(shell find program -maxdepth 1 -mindepth 1 -type d))))

.PHONY: all build-program build-kernel

all: initramfs.cpio kernel8.img

kernel8.img: kernel/kernel8.elf
	$(OBJCOPY) -O binary kernel/kernel8.elf $@

kernel/kernel8.elf: build-kernel
	$(MAKE) build-kernel

build-program:
	$(MAKE) -C program

build-kernel:
	$(MAKE) -C kernel

initramfs.cpio: $(PROGS) | build-program
	$(MAKE) $(PROGS) && \
	cd initramfs && find . | cpio -o -H newc > ../initramfs.cpio

$(PROGS):
initramfs/%: program/%.elf | initramfs
	$(OBJCOPY) -O binary $< $@

initramfs:
	mkdir $@

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C program clean
	rm -rf initramfs initramfs.cpio kernel8.img

run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -display none -serial null -serial stdio -semihosting

qemu-debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -display none -serial null -serial stdio -semihosting -S -s
