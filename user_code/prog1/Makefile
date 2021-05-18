.PHONY:all clean 
APP:=argv_test
LINKER:=linker.ld
CFLAGS:=-nostdinc -nostdlib -nostartfiles -fno-builtin
LIBS:=$(wildcard lib/*.c)
OBJS:=$(APP).o $(LIBS:.c=.o)

all:
	make clean
	make $(APP)
	cp $(APP) ../rootfs/$(APP)

$(APP):$(OBJS) start.o
	aarch64-linux-gnu-ld -T $(LINKER) -o tmp.elf $(OBJS) start.o
	aarch64-linux-gnu-objcopy -O binary tmp.elf $@
	rm tmp.elf

start.o: start.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@
	
%.o:%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@
	aarch64-linux-gnu-gcc $(CFLAGS) -S $<

clean:
	-rm $(APP)
	-rm $(OBJS)
	-rm *.s