#include "include/mini_uart.h"
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
char *dt_base_g = (char*)1; // Set to a non-zero value, so dt_base_g won't be in .bss

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

int strlen(char *str)
{ // '\0' doesn't count.
    int cnt;
    char *str_ptr;

    cnt = 0;
    str_ptr = str;
    while (*str_ptr++ != '\0')
        cnt++;
    return cnt;
}

int strcmp(char *str1, char *str2)
{
    int i;

    i = 0;
    while (str1[i] != '\0') {
        if (str1[i] != str2[i])
            return 1;
        i++;
    }
    if (str2[i] != '\0')
        return 1;
    return 0;
}

int strcmp_with_len(char *str1, char *str2, int len)
{ // compare two strings with at most len characters
    for (int i = 0; i < len; ++i) {
        if (str1[i] != str2[i])
            return 1;
        // if come to here, str1[i] == str2[i]
        if (str1[i] == '\0')
            return 0;
    }
    return 0;
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

int hex_string_to_int(char *hex_str, int len)
{
    int num, base;
    char *ch;

    num = 0;
    base = 1;
    ch = hex_str + len - 1;
    while(ch >= hex_str) {
        if (*ch >= 'A')
            num += (base*(10 + *ch - 'A'));
        else
            num += (base*(*ch - '0'));
        base *= 16;
        ch--;
    }
    return num;
}

unsigned long hex_string_to_unsigned_long(char *hex_str, int len)
{
    unsigned long num;
    int base;
    char *ch;

    num = 0;
    base = 1;
    ch = hex_str + len - 1;
    while(ch >= hex_str) {
        if (*ch >= 'A')
            num += (base*(10 + *ch - 'A'));
        else
            num += (base*(*ch - '0'));
        base *= 16;
        ch--;
    }
    return num;
}

int dec_string_to_int(char *dec_str, int len)
{
    unsigned long num;
    int base;
    char *ch;

    num = 0;
    base = 1;
    ch = dec_str + len - 1;
    while(ch >= dec_str) {
        num += (base*(*ch - '0'));
        base *= 10;
        ch--;
    }
    return num;
}

char *align_upper(char *addr, int alignment)
{
    char *res;
    int r = (unsigned long)addr % alignment;
    res = r ? addr + alignment - r : addr;
    return res;
}

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



#define FDT_BEGIN_NODE 0x00000001 // each node begins with FDT_BEGIN_NODE
#define FDT_END_NODE 0x00000002   // end with FDT_END_NODE. Node’s properties and subnodes are before the FDT_END_NOD
#define FDT_PROP 0x00000003 // describe property
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009 //  end of the structure block
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
struct fdt_header {
    uint32_t magic; // This field shall contain the value 0xd00dfeed (big-endian).
    uint32_t totalsize;
    uint32_t off_dt_struct;   // offset of the structure block from the beginning of the header.
    uint32_t off_dt_strings;  // offset of the strings block from the beginning of the header.
    uint32_t off_mem_rsvmap;  // offset of the mem.res. block from the beginning of the header.
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings; // length in bytes of the strings block section
    uint32_t size_dt_struct;  // length in bytes of the structure block section
};

uint32_t get_uint_endian(unsigned int *addr)
{ // Used to parse uint32_t in device tree object
    unsigned char *byte_ptr;

    // byte_ptr = (unsigned char*)(&(header->magic));
    byte_ptr = (unsigned char*)addr;
    unsigned int res = ((unsigned int)*byte_ptr << 24)  +
                        ((unsigned int)*(byte_ptr + 1) << 16)  +
                        ((unsigned int)*(byte_ptr + 2) << 8) +
                        ((unsigned int)*(byte_ptr + 3));
    return res;
}

void print_indent(unsigned int level)
{
    for (int i = 0; i < level; ++i)
        uart_send_string("    ");
    return;
}

int do_dtp()
{
    // unsigned long dt_base = 0x80000;
    char *dt_struct_ptr, *prop_data_end;
    unsigned int token, prop_data_len, prop_name_off, dt_level;

    uart_send_long((unsigned long)dt_base_g);
    uart_send_string("\r\n");
    struct fdt_header *header = (struct fdt_header*)dt_base_g;

    uart_send_string("Device tree magic: ");
    uart_send_uint(get_uint_endian(&(header->magic)));
    uart_send_string("\r\n");
    uart_send_string("Device tree version: ");
    uart_send_uint(get_uint_endian(&(header->version)));
    uart_send_string("\r\n");
    char *dt_struct_base = dt_base_g + get_uint_endian(&(header->off_dt_struct));
    char *dt_struct_end = dt_struct_base + get_uint_endian(&(header->size_dt_struct));
    char *dt_strings_base = dt_base_g + get_uint_endian(&(header->off_dt_strings));
    uart_send_string("string base: ");
    uart_send_string(dt_strings_base);
    uart_send_string("\r\n");

    dt_level = 0;
    dt_struct_ptr = dt_struct_base;
    while (dt_struct_ptr < dt_struct_end) {
        token = get_uint_endian((unsigned int*)dt_struct_ptr);
        dt_struct_ptr += sizeof(unsigned int);
        switch (token) {
            case FDT_BEGIN_NODE:
                print_indent(dt_level);
                uart_send_string("FDT_BEGIN_NODE\r\n");
                print_indent(dt_level);
                uart_send_string(dt_struct_ptr);
                uart_send_string("\r\n");
                dt_struct_ptr = align_upper(dt_struct_ptr + strlen(dt_struct_ptr) + 1, 4);
                dt_level++;
                break;
            case FDT_END_NODE:
                dt_level--;
                print_indent(dt_level);
                uart_send_string("FDT_END_NODE\r\n");
                break;
            case FDT_PROP:
                print_indent(dt_level);
                uart_send_string("FDT_PROP\r\n");
                prop_data_len = get_uint_endian((unsigned int*)dt_struct_ptr);
                dt_struct_ptr += sizeof(unsigned int);
                prop_name_off = get_uint_endian((unsigned int*)dt_struct_ptr);
                dt_struct_ptr += sizeof(unsigned int);
                prop_data_end = dt_struct_ptr + prop_data_len;
                print_indent(dt_level);
                uart_send_string(dt_strings_base + prop_name_off);
                uart_send_string(" : ");
                for (; dt_struct_ptr < prop_data_end; dt_struct_ptr++)
                    uart_send(*dt_struct_ptr);
                uart_send_string("\r\n");
                dt_struct_ptr = align_upper(dt_struct_ptr, 4);
                break;
            case FDT_NOP:
                print_indent(dt_level);
                uart_send_string("FDT_NOP\r\n");
                break;
            case FDT_END:
                print_indent(dt_level);
                uart_send_string("FDT_END\r\n");
                break;
            default:
                uart_send_string("Unrecognized token.\r\n");
                return 1;
        }
    }
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
    if (!strcmp(cmd, "dtp"))
        return do_dtp();
    if (!strcmp(cmd, "size"))
        return show_type_size();

    uart_send_string("Command '");
    uart_send_string(cmd);
    uart_send_string("' not found\r\n");
    return 0;
}


int kernel_main(char *sp)
{
    char cmd_buf[CMD_SIZE];

    uart_init();
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