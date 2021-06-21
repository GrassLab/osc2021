CC       := aarch64-linux-gnu-gcc
CXX      := aarch64-linux-gnu-g++
LD       := aarch64-linux-gnu-ld
OBJCOPY  := aarch64-linux-gnu-objcopy
CCFLAGS  := -std=gnu17
CXXFLAGS := -ffreestanding -nostdinc -nostdlib -nostdinc++ -nostartfiles -g -std=c++17 -MMD -I$(CURDIR)/include
export
PROGS    := $(patsubst %,initramfs/%,$(filter-out include lib,$(patsubst program/%,%,$(shell find program -maxdepth 1 -mindepth 1 -type d))))
ELFS     := $(patsubst initramfs/%,program/%.elf,$(PROGS))
QEMU     := qemu-system-aarch64
QEMU_ARG := -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -display none -serial null -serial stdio -semihosting -drive if=sd,file=nctuos.img,format=raw

.PHONY: all inter_kernel

all: initramfs.cpio kernel8.img

kernel8.img: kernel/kernel8.elf
	$(OBJCOPY) -O binary kernel/kernel8.elf $@

kernel/kernel8.elf: inter_kernel
	$(MAKE) -C kernel

inter_kernel:

initramfs.cpio: $(PROGS)
	cd initramfs && find . | cpio -o -H newc > ../initramfs.cpio

$(PROGS):
initramfs/%: program/%.elf | initramfs
	$(OBJCOPY) -O binary $< $@

$(ELFS): inter_elf
	@rm -f inter_elf

inter_elf:
	$(MAKE) -C program

initramfs:
	mkdir -p $@

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C program clean
	rm -rf initramfs initramfs.cpio kernel8.img

run: all
	$(QEMU) $(QEMU_ARG)

qemu-debug: all
	$(QEMU) $(QEMU_ARG) -S -s
