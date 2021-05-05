CC       := aarch64-linux-gnu-gcc
CXX      := aarch64-linux-gnu-g++
LD       := aarch64-linux-gnu-ld
OBJCOPY  := aarch64-linux-gnu-objcopy
CCFLAGS  := -std=gnu17
CXXFLAGS := -ffreestanding -nostdinc -nostdlib -nostdinc++ -nostartfiles -g -std=c++17 -fno-tree-loop-distribute-patterns -MMD
SRC      := $(shell find * -name '*.cpp')
ASM      := $(shell find * -name '*.S')
OBJ      := $(SRC:.cpp=.o) $(ASM:.S=.o)
DEP      := $(SRC:.cpp=.d)
PROG     := rootfs/hello

.PHONY: all clean qemu-debug run initramfs
all: kernel8.img $(PROG)
clean:
	find \( -name '*.o' -o -name '*.img' -o -name '*.elf' -o -name '*.d' \) -delete
	rm -rf rootfs/ initramfs.cpio
%.o: %.S
	$(CC) $(CCFLAGS) -c $< -o $@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
kernel8.elf: kernel8.ld $(filter kernel/%, $(OBJ))
	$(LD) -T $< -o $@ $(filter-out $<, $^)
kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@
hello.elf: program/hello/hello.ld $(filter program/hello/%, $(OBJ)) $(filter program/lib/%, $(OBJ))
	$(LD) -T $< -o $@ $(filter-out $<, $^)
rootfs/hello: hello.elf | rootfs
	$(OBJCOPY) -O binary $< $@
qemu-debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -display none -serial null -serial stdio -S -s
run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -display none -serial null -serial stdio
initramfs: rootfs
	cd rootfs/ && find . | cpio -o -H newc > ../initramfs.cpio
rootfs:
	mkdir rootfs
-include $(DEP)
