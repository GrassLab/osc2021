TARGET_EXEC := bootloader

SRCS := $(shell for /f tokens^=* %%i in ('where *.s')do echo %%~nxi)
OBJS := $(SRCS:.s=.o)

all: $(TARGET_EXEC).img

%.o: %.s
	aarch64-none-linux-gnu-gcc -c $<

$(TARGET_EXEC).img: $(OBJS) linker.ld
	aarch64-none-linux-gnu-ld -T linker.ld -o $(TARGET_EXEC).elf $(OBJS)
	aarch64-none-linux-gnu-objcopy -O binary $(TARGET_EXEC).elf $@

clean:
	del *.o $(TARGET_EXEC).elf $(TARGET_EXEC).img
