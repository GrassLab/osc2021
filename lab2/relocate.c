extern unsigned char _kernel_begin, _kernel_end, __boot_loader;  // variables defined in linker script

__attribute__((section(".text.relocate"))) void relocate() {
    // put this segment of code to '.text.relocate' to move the actual bootloader to another space
    // https://developer.arm.com/documentation/dui0491/i/Compiler-specific-Features/--attribute----section--name-----function-attribute

    unsigned long kernel_size = (&_kernel_end - &_kernel_begin);
    unsigned char *new_bl_address = (unsigned char*)&__boot_loader;
    unsigned char* bl = (unsigned char*)&_kernel_begin;

    // copy the content of BootLoader byte by byte
    while (kernel_size --) {
        *new_bl_address++ = *bl;
        *bl++ = 0;
    }

    void (*start)(void) = (void*) &__boot_loader;
    start();
}