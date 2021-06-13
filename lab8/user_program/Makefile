TARGET_EXEC := user_program

SRCS := $(shell for /f tokens^=* %%i in ('where *.c *.s')do echo %%~nxi)
OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

all: $(TARGET_EXEC).img

%.o: %.c
	aarch64-none-linux-gnu-gcc -c -ffreestanding -mgeneral-regs-only $<

%.o: %.s
	aarch64-none-linux-gnu-gcc -c -ffreestanding -mgeneral-regs-only $<

$(TARGET_EXEC).elf: $(OBJS)
	aarch64-none-linux-gnu-ld -static -o $(TARGET_EXEC).elf $(OBJS)

$(TARGET_EXEC).img: $(TARGET_EXEC).elf
	aarch64-none-linux-gnu-objcopy -O binary $(TARGET_EXEC).elf $(TARGET_EXEC).img

clean:
	del *.o $(TARGET_EXEC).elf $(TARGET_EXEC).img
