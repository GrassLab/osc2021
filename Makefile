CC = aarch64-linux-gnu-gcc
CFLAG = -fPIC -fno-stack-protector -nostdlib -nostartfiles -Wall -O2 -ffunction-sections
SF =
ifndef SF
	SF=start.S
endif
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
all:clean kernel8.img

s.o:$(SF)
	$(CC) $(CFLAG) -c $(SF) -o s.o
%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAG) -c $< -o $@

kernel8.img: s.o $(OBJS)
	aarch64-linux-gnu-ld s.o $(OBJS) -T linker.ld -nostdlib -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
	
clean:
	-rm kernel8.elf kernel8.img

