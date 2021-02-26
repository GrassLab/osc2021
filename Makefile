all: kernel8.img
	echo "Kernel8 image build completed!"

kernel8.img: kernel8.elf
# Build kernel image from elf file
	/usr/local/opt/llvm/bin/llvm-objcopy -O binary kernel8.elf kernel8.img

kernel8.elf: a.o linker.lds
# Linke elf file from object file
	/usr/local/opt/llvm/bin/ld.lld -T linker.lds -o kernel8.elf a.o

a.o: a.S
# Compile from asm to obj file
	clang -mcpu=cortex-a53 --target=aarch64-arm-linux-gnu a.S -o a.o

clean:
	rm -f kernel8.img kernel8.elf a.o
