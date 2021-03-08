.PHONY:all clean
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

all:
	for obj in $(OBJS);do\
		make $$obj;\
	done
%.o:%.c
	aarch64-linux-gnu-gcc -c $<

clean:
	-rm *.o
