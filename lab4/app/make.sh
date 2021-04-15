aarch64-linux-gnu-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -c app.S -o app.o
aarch64-linux-gnu-ld app.o  -T linker.ld -o app.elf
aarch64-linux-gnu-objcopy -O binary app.elf app_asm.img
aarch64-linux-gnu-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinclude -c app.c -o app.o
aarch64-linux-gnu-ld -nostdlib -nostartfiles app.o  -M -T linker.ld -o app.elf
aarch64-linux-gnu-objcopy -O binary app.elf app.img
rm app.o app.elf
cp app.img ../rootfs
cp app_asm.img ../rootfs
