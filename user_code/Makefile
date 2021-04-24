CC = aarch64-linux-gnu-gcc
CFLAG = -fPIC -Iinclude -fno-stack-protector -nostdlib -nostartfiles -ffunction-sections -fno-builtin

LINKER:=linker.ld
LIBS = $(wildcard lib/*.c)
OBJS = $(LIBS:.c=.o)

all: clean fork_test argv_test

%.o:%.c
	$(CC) $(CFLAG) -c $< -o $@

fork_test: fork_test.o $(OBJS)
	aarch64-linux-gnu-ld -T $(LINKER) -o tmp.elf -L fork_test.o $(OBJS) 
	aarch64-linux-gnu-objcopy -O binary tmp.elf $@
	rm tmp.elf

argv_test: argv_test.o $(OBJS)
	aarch64-linux-gnu-ld -T $(LINKER) -o tmp.elf $(OBJS) argv_test.o
	aarch64-linux-gnu-objcopy -O binary tmp.elf $@
	rm tmp.elf

clean:
	-rm lib/*.o 
	-rm *.o


