TARGET_EXEC := user_program

SRCS := $(shell for /f tokens^=* %%i in ('where *.c *.s')do echo %%~nxi)
OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

all: $(TARGET_EXEC).elf

%.o: %.c
	aarch64-none-linux-gnu-gcc -c -ffreestanding $<

%.o: %.s
	aarch64-none-linux-gnu-gcc -c -ffreestanding $<

$(TARGET_EXEC).elf: $(OBJS)
	aarch64-none-linux-gnu-ld -static -o $(TARGET_EXEC).elf $(OBJS)

clean:
	del *.o $(TARGET_EXEC).elf
