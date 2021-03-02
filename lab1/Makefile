TARGET_EXEC := kernel8

SRCS := $(shell for /f tokens^=* %%i in ('where *.c *.s')do echo %%~nxi)
OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

all: $(TARGET_EXEC).img

%.o: %.c
	aarch64-none-linux-gnu-gcc -c $(flagcc) $<

%.o: %.s
	aarch64-none-linux-gnu-gcc -c $(flagcc) $<

$(TARGET_EXEC).img: $(OBJS) linker.ld
	aarch64-none-linux-gnu-ld -T linker.ld -o $(TARGET_EXEC).elf $(OBJS)
	aarch64-none-linux-gnu-objcopy -O binary $(TARGET_EXEC).elf $@

clean:
	del *.o $(TARGET_EXEC).elf $(TARGET_EXEC).img

run: $(TARGET_EXEC).img
	qemu-system-aarch64 -M raspi3 -kernel $(TARGET_EXEC).img -display none -serial null -serial stdio

debug: flagcc = -ggdb

debug: clean $(TARGET_EXEC).img
	qemu-system-aarch64 -M raspi3 -kernel $(TARGET_EXEC).img -display none -serial null -serial stdio -S -s