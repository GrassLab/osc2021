.PHONY:all clean
CFLAGS:=-nostdinc -nostdlib -nostartfiles
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

all:
	for obj in $(OBJS);do\
		make $$obj;\
	done
%.o:%.c %.h
	aarch64-linux-gnu-gcc $(CFLAGS) -c $<
	aarch64-linux-gnu-gcc $(CFLAGS) -S $<

clean:
	-rm *.o
	-rm *.s