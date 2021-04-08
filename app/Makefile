LINKER:=linker.ld
CFLAGS:=-nostdinc -nostdlib -nostartfiles

app.img:app.o
	aarch64-linux-gnu-ld -T $(LINKER) -o app.elf $<
	aarch64-linux-gnu-objcopy -O binary app.elf $@
	cp $@ ../rootfs/$@

app.o:app.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $<

clean:
	rm app.img app.elf app.o