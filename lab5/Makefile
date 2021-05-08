OBJCOPY   := aarch64-linux-gnu-objcopy
PROGS     := $(patsubst %,initramfs/%,$(filter-out include lib,$(patsubst program/%,%,$(shell find program -maxdepth 1 -mindepth 1 -type d))))

.PHONY: all build

all: initramfs.cpio
build:
	$(MAKE) -C kernel && $(OBJCOPY) -O binary kernel/kernel8.elf kernel8.img
	$(MAKE) -C program

initramfs.cpio: $(PROGS) | build
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
	rm -rf initramfs initramfs.cpio

run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -display none -serial null -serial stdio -semihosting

qemu-debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -display none -serial null -serial stdio -semihosting -S -s
