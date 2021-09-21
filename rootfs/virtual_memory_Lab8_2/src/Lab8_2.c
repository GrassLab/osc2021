#include "printf.h"
#include "sys.h"

int main(int argc, char **argv) {
    printf("[Lab8_2.c] Here exec syscall success!\n");

    call_sys_exit();

    return -1;
}