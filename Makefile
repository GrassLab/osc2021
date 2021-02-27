ARMGNU = aarch64-linux-gnu
C_FLAGS = -Wall -Wextra -Wshadow -Wconversion -nostdlib -nostartfiles

KERNEL_C_SOURCE_FILES = main.c uart.c
KERNEL_C_OBJECT_FILES = $(KERNEL_C_SOURCE_FILES:.c=.o)
KERNEL_ASSEMBLY_SOURCE_FILES = start.S
KERNEL_ASSEMBLY_OBJECT_FILES = $(KERNEL_ASSEMBLY_SOURCE_FILES:.S=.o)

all: kernel8.img

kerenl8.img: kernel8.elf
		${ARMGNU}-objcopy -O binary kernel8.elf kernel8.img

kernel8.elf: ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES} 
		${ARMGNU}-ld -T linker.ld -o kernel8.elf ${KERNEL_ASSEMBLY_OBJECT_FILES} ${KERNEL_C_OBJECT_FILES}

%.o: %.c 
		${ARMGNU}-cc -c ${C_FLAGS} $^ -o $@

%.o: %.S 
		${ARMGNU}-cc -c $^ -o $@

clean: rm -rf *.o *.elf *img

run: kernel8.img
		qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

debug: kernel8.img 
		qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s