#include "lib.h"
#include "string.h"
#include "args.h"

void sleep (unsigned long time) {
    nanosleep(time * 1000000000);
}

#define print_buffer_size 0x100

void printf (char *format, ...) {
    char buffer[print_buffer_size];

    va_list ap;
    va_start(ap, format);
    vsnprintf(buffer, print_buffer_size, format, ap);
    va_end(ap);

    write(0, buffer, sizeof(buffer));
}

void show_stack () {
    unsigned long stack, kstack, sp;
    stack_info(&stack, &kstack, &sp);
    printf("id: %d, stack: %x, kstack %x, sp: %x\n", get_pid(), stack, kstack, sp);
}
