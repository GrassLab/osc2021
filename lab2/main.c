#include "include/mini_uart.h"
#include "include/mm.h"
#include "include/cutils.h"
#include "include/dtp.h"
#include "include/test.h"
// #include "include/initramfs.h"
#include "utils.h"
#define CMD_SIZE 64
#define FILE_NAME_SIZE 64
#define INITRAMFS_BASE 0x20000000 // rpi3
// #define INITRAMFS_BASE 0x8000000 // QEMU
#define PM_PASSWORD 0x5a000000
#define PM_RSTC 0x3F10001c
#define PM_WDOG 0x3F100024

int BSSTEST = 0;
extern char *dt_base_g;

char *cmd_lst[][2] = {
    { "help   ", "list all commands"},
    { "hello  ", "print hello world"},
    { "reboot ", "reboot"},
    { "cat    ", "show file contents"},
    { "ls     ", "show all file"},
    { "relo   ", "Relocate bootloader"},
    { "load   ", "Load image from host to pi, then jump to it"},
    { "showmem", "show memory contents"},
    { "dtp    ", "device tree parse"},
    { "size   ", "show type size"},
    { "mm     ", "show memory management"},
    { "testbs ", "test buddy system"},
    { "testks ", "test kmalloc system"},
    { "testsms", "test startup memory system"},
    { "eocl", "This won't print out (end of cmd list)"}
};

/* Variable defined in linker.ld: bss_end, start_begin
 * In theory, the extern can be any primitive data type.
 * For reasons which I am unaware, the convention is to
 * use a char.
 * (extern char *bss_end, *start_begin;) will fail, and
 * I still don't know why.
 * https://sourceware.org/binutils/docs/ld/Source-Code-Reference.html
 */
extern char bss_end[];
extern char start_begin[];

void reset(int tick){ // reboot after watchdog timer expire
    put32(PM_RSTC, PM_PASSWORD | 0x20); // full reset
    put32(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
}

int do_reboot(void)
{
    reset(1);
    uart_send_string("Rebooting...\r\n");
    return 0;
}

void set_dt_base(char *dt_base)
{
    dt_base_g = dt_base;
    return;
}

void memzero(char *bss_begin, unsigned long len)
{
    char *mem_ptr = bss_begin;
    for (int i = 0; i < len; ++i)
        *mem_ptr++ = 0;
    return;
}

int do_help(void)
{
    for (int i = 0; strcmp(cmd_lst[i][0], "eocl"); ++i) {
        uart_send_string(cmd_lst[i][0]);
        uart_send_string(" : ");
        uart_send_string(cmd_lst[i][1]);
        uart_send_string("\r\n");
    }
    return 0;
}

int do_hello(void)
{
    uart_send_string("Hello World!\r\n");
    return 0;
}

struct cpio_newc_header {
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
};


int cat_file_initramfs()
{
    char file_name_buf[FILE_NAME_SIZE];
    struct cpio_newc_header* ent;
    int filesize, namesize;
    char *name_start, *data_start;

    uart_send_string("Please enter file path: ");
    read_line(file_name_buf, FILE_NAME_SIZE);

    ent = (struct cpio_newc_header*)INITRAMFS_BASE;
    while (1)
    {
        // uart_send_string("hey");
        namesize = hex_string_to_int(ent->c_namesize, 8);
        filesize = hex_string_to_int(ent->c_filesize, 8);
        name_start = ((char *)ent) + sizeof(struct cpio_newc_header);
        data_start = align_upper(name_start + namesize, 4);
        if (!strcmp(file_name_buf, name_start)) {
            if (!filesize)
                return 0;
            uart_send_string(data_start);
            uart_send_string("\r\n");
            return 0;
        }
        ent = (struct cpio_newc_header*)align_upper(data_start + filesize, 4);

        if (!strcmp(name_start, "TRAILER!!!"))
            break;
    }
    uart_send_string("cat: ");
    uart_send_string(file_name_buf);
    uart_send_string(": No such file or directory\r\n");
    return 1;
}

int ls_initramfs()
{
    struct cpio_newc_header* ent;
    int filesize, namesize;
    char *name_start, *data_start;

    ent = (struct cpio_newc_header*)INITRAMFS_BASE;
    while (1) {
        namesize = hex_string_to_int(ent->c_namesize, 8);
        filesize = hex_string_to_int(ent->c_filesize, 8);
        name_start = ((char *)ent) + sizeof(struct cpio_newc_header);
        if (!strcmp(name_start, "TRAILER!!!"))
            break;
        data_start = align_upper(name_start + namesize, 4);
        uart_send_string(name_start);
        uart_send_string("\r\n");
        ent = (struct cpio_newc_header*)align_upper(data_start + filesize, 4);
    }
    return 0;
}

int do_showmem(char *cmd)
{ // showmem 60000 200
    char *addr_ptr, *cmd_ptr, *arg1, *arg2;
    int arg1_len, arg2_len, len;
    unsigned long addr;

    cmd_ptr = cmd;
    while (*cmd_ptr++ != ' ');
    arg1 = cmd_ptr;
    arg1_len = 0;
    while (*cmd_ptr++ != ' ')
        arg1_len++;
    arg2 = cmd_ptr;
    arg2_len = 0;
    while (*cmd_ptr++ != '\0')
        arg2_len++;
    addr = hex_string_to_unsigned_long(arg1, arg1_len);
    addr_ptr = (char*)addr;
    len = dec_string_to_int(arg2, arg2_len);

    // addr_ptr = dt_base_g;
    // len = 500;
    for (int i = 0; i < len; ++i) {
        uart_send(addr_ptr[i]);
        uart_send_string("");
    }
    uart_send_string("\r\n");
    return 0;
}

int kernel_main(char *sp);

void bootloader_relocate()
{ // Copy bootloader, then jump to the new bootloader.
    unsigned long bootloader_new_addr;
    char *dest, *start, *end;
    char buf[100];
    int address_len;
    long dest_offset;

    uart_send_string("Please enter relocation address(hex without '0x'): ");
    address_len = read_line(buf, 100);
    bootloader_new_addr = hex_string_to_unsigned_long(buf, address_len);
    // uart_send_long(bootloader_new_addr);
    dest = (char*)bootloader_new_addr;
    start = start_begin;
    end = bss_end;

    uart_send_string("Start relocating...\r\n");
    while (start <= end) {
        *dest = *start; // put8(dest, *start)
        start++;
        dest++;
    }
    uart_send_string("Relocation complete!\r\n");
    uart_send_string("Prepare to jump to new bootloader...\r\n");
    int (*kernel_main_ptr)(char*) = kernel_main;
    dest_offset = (char*)kernel_main_ptr - start_begin;
    branch_to_address(dt_base_g, (char*)bootloader_new_addr + dest_offset);
}


int kernel_load_and_jump(void)
{
    long kernel_size;
    char byte;
    unsigned char checksum;

    // uart_send_string("Please enter address you'd like to load kernel8.img: ");
    unsigned long kernel_addr_hex = 0x80000;
    char *kernel_addr = (char*)kernel_addr_hex;
    uart_send_string("Please enter size of kernel8.img: ");
    kernel_size = uart_read_int();
    uart_send_string("You can start sending kernel8.img now...\r\n");
    checksum = 0;
    for (int i = 0; i < kernel_size; ++i) {
        byte = uart_recv();
        kernel_addr[i] = byte;
        checksum += byte;
    }
    uart_send_string("Checksum: ");
    uart_send_int(checksum);
    uart_send_string("\r\n");
    uart_send_string("Receiving complete!\r\n");

    uart_send_string("Prepare to jump to kernel...\r\n");
    branch_to_address(dt_base_g, kernel_addr);
    // do_showmem();
    return 0;
}

int show_type_size()
{
    uart_send_string("sizeof(int)= ");
    uart_send_int(sizeof(int));
    uart_send_string("\r\n");
    return 0;
}

int cmd_handler(char *cmd)
{
    if (!strcmp(cmd, "help"))
        return do_help();
    if (!strcmp(cmd, "hello"))
        return do_hello();
    if (!strcmp(cmd, "reboot"))
        return do_reboot();
    if (!strcmp(cmd, "cat"))
        return cat_file_initramfs();
    if (!strcmp(cmd, "ls"))
        return ls_initramfs();
    if (!strcmp(cmd, "relo"))
        bootloader_relocate();
    if (!strcmp(cmd, "load"))
        kernel_load_and_jump(); // Won't come back
    if (!strcmp_with_len(cmd, "showmem", 7))
        return do_showmem(cmd);
    // if (!strcmp(cmd, "dtp"))
    //     return do_dtp();
    if (!strcmp(cmd, "size"))
        return show_type_size();
    if (!strcmp(cmd, "mm"))
        return show_mm();
    if (!strcmp(cmd, "testbs"))
        return _test_buddy_system();
    if (!strcmp(cmd, "testks"))
        return _test_kmalloc_system();
    if (!strcmp(cmd, "testsms"))
        return _test_startup_alloc_system();
    if (!strcmp(cmd, "lz")){
        uart_send_int(lead_zero(8));
        uart_send_string("\r\n");
        return 0;
    }
    uart_send_string("Command '");
    uart_send_string(cmd);
    uart_send_string("' not found\r\n");
    return 0;
}


int kernel_main(char *sp)
{
    char cmd_buf[CMD_SIZE];

    do_dtp(uart_probe);
    // uart_init();
    rd_init();
    dynamic_mem_init();
    uart_send_string("Welcome to RPI3-OS\r\n");
    while (1) {
        uart_send_string("user@rpi3:~$ ");
        read_line(cmd_buf, CMD_SIZE);
        if (!strlen(cmd_buf))  // User input nothing but Enter
            continue;
        cmd_handler(cmd_buf);
    }
    return 0;
}