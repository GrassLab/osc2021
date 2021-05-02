.PHONY:all clean 
APP:=app2
LINKER:=linker.ld
CFLAGS:=-nostdinc -nostdlib -nostartfiles
LIBS:=$(wildcard inc/*.c)
OBJS:=$(APP).o $(LIBS:.c=.o)

all:
	make $(APP)
	cp $(APP) ../rootfs/$(APP)
	cd ..&&make

$(APP):$(OBJS)
	aarch64-linux-gnu-ld -T $(LINKER) -o tmp.elf $(OBJS)
	aarch64-linux-gnu-objcopy -O binary tmp.elf $@
	rm tmp.elf

%.o:%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@
	aarch64-linux-gnu-gcc $(CFLAGS) -S $<

clean:
	-rm $(APP)
	-rm $(OBJS)
	-rm *.s
