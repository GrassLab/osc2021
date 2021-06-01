#include "include/mini_uart.h"
#include "include/mm.h"
#include "include/cutils.h"
#include "include/dtp.h"
#include "include/test.h"
#include "include/cirq.h"
#include "include/csched.h"
#include "include/syslib.h"
#include "include/vfs.h"
#include "include/tmpfs.h"
#include "include/procfs.h"
#include "include/fat32.h"
#include "include/sdhost.h"
#include "include/entry.h"
// #include "include/initramfs.h"
#include "utils.h"
#define CMD_SIZE 64
#define FILE_NAME_SIZE 64
#define PM_PASSWORD (0x5a000000 + 0xffff000000000000)
#define PM_RSTC (0x3F10001c + 0xffff000000000000)
#define PM_WDOG (0x3F100024 + 0xffff000000000000)

int BSSTEST = 0;
extern char *dt_base_g;

char *cmd_lst[][2] = {
    { "help       ", "list all commands"},
    { "hello      ", "print hello world"},
    { "reboot     ", "reboot"},
    { "cat        ", "show file contents"},
    { "run        ", "run user program"},
    { "ls         ", "show all file"},
    { "relo       ", "Relocate bootloader"},
    { "load       ", "Load image from host to pi, then jump to it"},
    { "showmem    ", "showmem start length"},
    { "size       ", "show type size"},
    { "mm         ", "show memory management"},
    { "testbs     ", "test buddy system"},
    { "testks     ", "test kmalloc system"},
    { "testsms    ", "test startup memory system"},
    { "el         ", "Get current EL"},
    { "setTimeout ", "setTimeout MESSAGE SECONDS"},
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

void set_dt_base(unsigned long dt_base)
{
    // dt_base_g = (char*)(dt_base + 0xffff000000000000);
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

int cat_file_initramfs()
{
    char file_name_buf[FILE_NAME_SIZE];
    struct cpio_newc_header* ent;
    int filesize, namesize;
    char *name_start, *data_start;

    uart_send_string("Please enter file path: ");
    read_line_low_power(file_name_buf, FILE_NAME_SIZE);
    // read_line(file_name_buf, FILE_NAME_SIZE);
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


int run_initramfs()
{
    char file_name_buf[FILE_NAME_SIZE];

    uart_send_string("Please enter file path: ");
    read_line_low_power(file_name_buf, FILE_NAME_SIZE);
    exec(file_name_buf, 0);
    return 1;
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
    address_len = read_line_low_power(buf, 100);
    // address_len = read_line(buf, 100);
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
    uart_send_string("sizeof(short)= ");
    uart_send_int(sizeof(short));
    uart_send_string("\r\n");
    return 0;
}

int show_sys_reg()
{
    int el = get_el();
    uart_send_string("Current EL: ");
    uart_send_int(el);
    uart_send_string("\r\n");
    uart_send_string("sctlr_el1: ");
    uart_send_uint(get_sctlr_el1());
    uart_send_string("\r\n");
    uart_send_string("cntfrq_el0: ");
    uart_send_uint(get_cntfrq_el0());
    uart_send_string("\r\n");

    if (el == 2) {
        uart_send_string("hcr_el2: ");
        uart_send_ulong(get_hcr_el2());
        uart_send_string("\r\n");
        uart_send_string("spsr_el2: ");
        uart_send_uint(get_spsr_el2());
        uart_send_string("\r\n");
    }
    return 0;
}

void *wakeup_mesg(void* arg)
{
    char *mesg = (char*)arg;
    uart_send_string("\r\n");
    uart_send_string(mesg);
    
    return 0; // null
}
char globl_mesg[100];

int do_setTimeout(char *cmd)
{ // setTimeout MESSAGE second
    char *cmd_ptr, *arg2;
    int arg1_len, arg2_len, seconds;

    cmd_ptr = cmd;
    while (*cmd_ptr++ != ' ');
    globl_mesg[0] = '\0';
    arg1_len = 0;
    while (*cmd_ptr != ' ') {
        globl_mesg[arg1_len] = *cmd_ptr;
        cmd_ptr++;
        arg1_len++;
    }
    globl_mesg[arg1_len] = '\0';  // make arg1 a null-end string
    arg2 = ++cmd_ptr;
    arg2_len = 0;
    while (*cmd_ptr++ != '\0')
        arg2_len++;
    seconds = dec_string_to_int(arg2, arg2_len);
    // Now, globl_mesg is MESSAGE and arg2 is SECONDS
    tqe_add(seconds * TICKS_FOR_ITR, wakeup_mesg, (void*)globl_mesg);

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
    if (!strcmp(cmd, "run"))
        return run_initramfs();
    if (!strcmp(cmd, "ls"))
        return ls_initramfs();
    if (!strcmp(cmd, "relo"))
        bootloader_relocate();
    if (!strcmp(cmd, "load"))
        kernel_load_and_jump(); // Won't come back
    if (!strcmp_with_len(cmd, "showmem", 7))
        return do_showmem(cmd);
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
    if (!strcmp(cmd, "el"))
        return show_sys_reg();
    if (!strcmp_with_len(cmd, "setTimeout", 10))
        return do_setTimeout(cmd);
    if (!strcmp(cmd, "eirq")) {
        enable_irq();
        return 0;
    }
    if (!strcmp(cmd, "dirq")) {
        disable_irq();
        return 0;
    }

    uart_send_string("Command '");
    uart_send_string(cmd);
    uart_send_string("' not found\r\n");
    return 0;
}


int ls(char *path) {
    int fd = open(path, 0);
    char name[100];
    int size;
    // Modify the for loop to iterate
    // the directory entries of the
    // opened directory.
    struct dentry dent;
    for(;;) {
        if (stat_and_next(fd, &dent))
            break;
        uart_send_string(dent.name);
        uart_send_string("  :  ");
        uart_send_int(dent.size);
        uart_send_string("\r\n");
    }
    close(fd);
    return 0;
}

extern struct task task_pool[];
extern struct task *current;


void udh_1()
{
    uart_send_string("From udh_1\r\n");
}

void test_thread_1(char arg[])
{
    // signal(0, (unsigned long)udh_1);
    while (1) {
        for(int i = 0; i < 10; ++i) {
            uart_write(arg, 5);
            delay(1000000);
        }
        // schedule();
    }
    exit();
}

void test_thread_2(char arg[])
{
    while (1) {
        for(int i = 0; i < 10; ++i) {
            uart_write(arg, 5);
            delay(1000000);
        }
        sleep(10);
        // schedule();
    }
    exit();
}

int fork_test(void) {
    uart_send_string("Fork Test, pid ");
    uart_send_int(get_pid());
    uart_send_string("\r\n");
    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) { // child
        uart_send_string("pid: ");
        uart_send_int(get_pid());
        uart_send_string(", cnt: ");
        uart_send_int(cnt);
        uart_send_string(", ptr: ");
        uart_send_ulong((unsigned long)&cnt);
        uart_send_string("\r\n");
        ++cnt;
        fork();
        while (cnt < 5) {
            uart_send_string("pid: ");
            uart_send_int(get_pid());
            uart_send_string(", cnt: ");
            uart_send_int(cnt);
            uart_send_string(", ptr: ");
            uart_send_ulong((unsigned long)&cnt);
            uart_send_string("\r\n");
            delay(1000000);
            ++cnt;
        }
        uart_send_string("Child end\r\n");
    } else {
        uart_send_string("parent here, pid ");
        uart_send_int(get_pid());
        uart_send_string(", child ");
        uart_send_int(ret);
        uart_send_string("\r\n");
        uart_send_string("Parent end\r\n");
    }
    exit();

    while(1);
}

void user_logic(int argc, char **argv)
{
    int pid = get_pid();
    uart_send_string("Argv Test, pid ");
    uart_send_int(pid);
    uart_send_string("\r\n");
    for (int i = 0; i < argc; ++i) {
        uart_send_string(argv[i]);
        uart_send_string("\r\n");
    }
    kill(2, 0);
    useless();
    // exit();
    char *fork_argv[2];
    fork_argv[0] = "fork_test";
    fork_argv[1] = 0;
    exec((unsigned long)fork_test, fork_argv);
}

void user_logic_2(int argc, char **argv)
{
    char buf[100];
    int a = open("/hello", O_CREAT);
    int b = open("/world", O_CREAT);
    write(a, "Hello ", 6);
    write(b, "World!", 6);
    close(a);
    close(b);
    b = open("/hello", 0);
    a = open("/world", 0);
    int sz;
    sz = read(b, buf, 100);
    sz += read(a, buf + sz, 100);
    buf[sz] = '\0';
    uart_send_string(buf); // should be Hello World!
    uart_send_string("\r\n");
    exit();
}

void user_logic_3(int argc, char **argv)
{
    ls(".");
    exit();
}

void user_logic_4(int argc, char **argv)
{
    char buf[8];
    mkdir("/mnt", 0);
    ls(".");
    int fd = open("/mnt/a.txt", O_CREAT);
    write(fd, "Hi", 2);
        uart_send_string("ASSERT: fd >= 0\r\n");
    close(fd);
    chdir("/mnt");
    // ls("..");
    fd = open("./a.txt", 0);
    // assert(fd >= 0);
    if (fd >= 0)
        uart_send_string("ASSERT: fd >= 0\r\n");
    read(fd, buf, 2);
    // assert(strncmp(buf, "Hi", 2) == 0);
    if (strcmp_with_len(buf, "Hi", 2) == 0)
        uart_send_string("ASSERT: strncmp(buf, \"Hi\", 2) == 0\r\n");


    chdir("..");
    mount("tmpfs", "/mnt", "tmpfs");
    fd = open("/mnt/a.txt", 0);
    // assert(fd < 0);
    if (fd < 0)
        uart_send_string("ASSERT: fd < 0\r\n");
    umount("/mnt");
    fd = open("/mnt/a.txt", 0);
    // assert(fd >= 0);
    if (fd >= 0)
        uart_send_string("ASSERT: fd >= 0\r\n");
    read(fd, buf, 2);
    // assert(strncmp(buf, "Hi", 2) == 0);
    if (strcmp_with_len(buf, "Hi", 2) == 0)
        uart_send_string("ASSERT: strncmp(buf, \"Hi\", 2) == 0\r\n");
    exit();
}

void user_logic_5(int argc, char **argv)
{
    int fd;
    char buf[16];
    mkdir("/proc", 0);
    mount("procfs", "/proc", "procfs");
    if ((fd = open("/proc/switch", 0)) < 0)
        uart_send_string("Fail\r\n");
    write(fd, "0", 1);
    close(fd);

    fd = open("/proc/hello", 0);
    int sz = read(fd, buf, 16);
    buf[sz] = '\0';
    // printf("%s\n", buf); // should be hello
    uart_send_string(buf);
    uart_send_string("\r\n");
    close(fd);

    fd = open("/proc/switch", 0);
    write(fd, "1", 1);
    close(fd);

    fd = open("/proc/hello", 0);
    sz = read(fd, buf, 16);
    buf[sz] = '\0';
    // printf("%s\n", buf); //should be HELLO
    uart_send_string(buf);
    uart_send_string("\r\n");
    close(fd);

    if ((fd = open("/proc/1/status", 0)) < 0) // choose a created process's id here
        uart_send_string("Open failed\r\n");
    // ls("/proc/2");
    sz = read(fd, buf, 16);
    buf[sz] = '\0';
    // printf("%s\n", buf); // process's status.
    uart_send_string(buf);
    uart_send_string("\r\n");
    close(fd);

    fd = open("/proc/999/status", 0); // choose a non-existed process's id here
    // assert(fd < 0);
    if (fd < 0)
        uart_send_string("fd < 0\r\n");
    exit();
}

void user_logic_6(int argc, char **argv)
{
    int fd;
    char buf[17];// = "This is NOT COOL";
    uart_send_string("From user_logic_6: A\r\n");

    ls(".");
    // fd = open("/HALO.PY", O_CREAT);
    if ((fd = open("/HALO.PY", 0)) < 0) {
        uart_send_string("Error: fd open fail\r\n");
        exit();
    }
    read(fd, buf, 32);
    buf[20] = '\0';
    uart_send_string(buf);
    write(fd, buf, 19);
    // sync();
    exit();
}

void user_logic_7(int argc, char **argv)
{
    int fd;
    char buf[8] = "HELLO!";

    mknod("/uartdev", 0);
    if ((fd = open("/uartdev", 0)) < 0)
        uart_send_string("Error: open fail\r\n");
    buf[7] = '\0';
    // read(fd, buf, 4);
    write(fd, buf, 8);
    // uart_send_string(buf);

    exit();
}

void user_thread()
{
    char *argv[7];
    argv[0] = "argv_test";
    argv[1] = "aux";
    argv[2] = "-o";
    argv[3] = "-tsk";
    argv[4] = "haha";
    argv[5] = "shutup\r\n";
    argv[6] = 0;
    uart_send_string("From user_thread: A\r\n");
    // char *inva = 0x00ff0000ffff0000;
    // *inva = 'k';
    // argv = {"user_logic", "hello1", "-aux", 0};
    exec("bin/argv_test", argv);
    // exec("bin/fork_test", argv);
    // exit();
}


void user_thread_2()
{ // test function version exec
    uart_send_string("From user_thread_2\r\n");
    char *argv[7];
    argv[0] = "ls";
    argv[1] = "/bin";
    argv[2] = "-o";
    argv[3] = "-tsk";
    argv[4] = "haha";
    argv[5] = "shutup\r\n";
    argv[6] = 0;
    // current->sig.sigpend = 1; // DEMO: raise signal itself
    // current->sig.user_handler[0] = (unsigned long)udh_1;
    // exec((unsigned long)user_logic_2, argv); // DEMO:
    exec((unsigned long)user_logic_7, argv);

}


void idle()
{
    struct task *walk;

    while (1) {
        // uart_send_string("[IDLE]\r\n");
        // delay(10000000);
        for (int i = 0; i < MAX_TASK_NR; ++i) {
            walk = &task_pool[i];
            if (walk->free == 0 && walk->status == TASK_ZOMBIE) {
                uart_send_string("From idle: Find zombie and cleared.\r\n");
                kfree(walk->kernel_stack_page);
                destroy_pgd(walk->mm);
                // kfree(walk->user_stack_page);
                walk->free = 1;
            }
        }

        schedule();
    }
}


int sys_exec(char *filename, char *const argv[])
{
    unsigned long func_addr;
    struct mm_struct *mm;
    struct cpio_newc_header* ent;
    int filesize, namesize;
    char *name_start, *data_start;

    mm = current->mm;
    ent = (struct cpio_newc_header*)INITRAMFS_BASE;
    while (1)
    {
        namesize = hex_string_to_int(ent->c_namesize, 8);
        filesize = hex_string_to_int(ent->c_filesize, 8);
        name_start = ((char *)ent) + sizeof(struct cpio_newc_header);
        data_start = align_upper(name_start + namesize, 4);
        if (!strcmp(filename, name_start)) { // Find the file
            mm->cpio_start = data_start;
            mm->cpio_size = filesize;
            if (!filesize)
                return 0;
            create_user_space(mm); // virtual space: 0 ~ filesize
            break;
        }
        if (!strcmp(name_start, "TRAILER!!!")) {
            // Still don't find file at the end.
            uart_send_string("do_exec: ");
            uart_send_string(filename);
            uart_send_string(": No such file or directory\r\n");
            return 1;
        }
        ent = (struct cpio_newc_header*)align_upper(data_start + filesize, 4);
    }


    int argc, len;
    char *user_sp;
    char **argv_fake, **argv_fake_start;
    struct trap_frame *cur_trap_frame;
delay(10000000);
    /* Get argc, not include null terminate */
    argc = 0;
    while (argv[argc])
        argc++;
    argv_fake = (char**)kmalloc(0x1000);
    user_sp = current->usp;
    for (int i = argc - 1; i >= 0; --i) {
        len = strlen(argv[i]) + 1; // including '\0'
        user_sp -= len;
        argv_fake[i] = user_sp;
        memcpy(user_sp, argv[i], len);
    }
    user_sp -= sizeof(char*); // NULL pointer
    user_sp = align_down(user_sp, 0x8); // or pi will fail
    *((char**)user_sp) = (char*)0;
    for (int i = argc - 1; i >= 0; --i) {
        user_sp -= sizeof(char*);
        *((char**)user_sp) = argv_fake[i];
    }
    // TODO: argv_fake_start: this is
    // temporary. cause now  I don't
    // have solution to conquer the
    // pushing stack issue when
    // starting of function call.
    argv_fake_start = (char**)user_sp;
    user_sp -= sizeof(char**); // char** argv
    *((char**)user_sp) = user_sp + sizeof(char**);
    user_sp -= sizeof(int); // argc
    *((int*)user_sp) = argc;
    kfree((char*)argv_fake);
    current->usp = align_down(user_sp, 0x10);

    cur_trap_frame = (struct trap_frame *)(get_trap_frame(current) + 0xffff000000000000);
    cur_trap_frame->regs[0] = (unsigned long)argc;
    cur_trap_frame->regs[1] = (unsigned long)argv_fake_start;
    cur_trap_frame->sp_el0 = (unsigned long)current->usp;
    cur_trap_frame->elr_el1 = 0x0; // user space start from 0.
    cur_trap_frame->spsr_el1 = 0x0; //0x0 0x3C5 enable_irq

    return argc;  // Geniusly
}

// int sys_exec(unsigned long func_addr, char *const argv[])
// {
//     int argc, len;
//     char *user_sp;
//     char **argv_fake, **argv_fake_start;
//     struct trap_frame *cur_trap_frame;
// delay(10000000);
//     /* Get argc, not include null terminate */
//     argc = 0;
//     while (argv[argc])
//         argc++;
//     // argv_fake = kmalloc(argc*sizeof(char*));
//     argv_fake = (char**)kmalloc(0x1000);
//     user_sp = current->user_stack_page + 0x1000;
//     for (int i = argc - 1; i >= 0; --i) {
//         len = strlen(argv[i]) + 1; // including '\0'
//         user_sp -= len;
//         argv_fake[i] = user_sp;
//         memcpy(user_sp, argv[i], len);
//     }
//     user_sp -= sizeof(char*); // NULL pointer
//     user_sp = align_down(user_sp, 0x8); // or pi will fail
//     *((char**)user_sp) = (char*)0;
//     for (int i = argc - 1; i >= 0; --i) {
//         user_sp -= sizeof(char*);
//         *((char**)user_sp) = argv_fake[i];
//     }
//     // TODO: argv_fake_start: this is
//     // temporary. cause now  I don't
//     // have solution to conquer the
//     // pushing stack issue when
//     // starting of function call.
//     argv_fake_start = (char**)user_sp;
//     user_sp -= sizeof(char**); // char** argv
//     *((char**)user_sp) = user_sp + sizeof(char**);
//     user_sp -= sizeof(int); // argc
//     *((int*)user_sp) = argc;
//     kfree((char*)argv_fake);
//     current->usp = align_down(user_sp, 0x10);

//     cur_trap_frame = get_trap_frame(current);
//     cur_trap_frame->regs[0] = (unsigned long)argc;
//     cur_trap_frame->regs[1] = (unsigned long)argv_fake_start;
//     cur_trap_frame->sp_el0 = (unsigned long)current->usp;
//     cur_trap_frame->elr_el1 = func_addr;
//     cur_trap_frame->spsr_el1 = 0x3C5; // enable_irq

//     // set_user_program((char*)func_addr, current->usp, argc,
//     //     argv_fake_start, current->kernel_stack_page + 0x1000);
//     // never come back again.
//     return argc;  // Geniusly
// }


int shell()
{
    char cmd_buf[CMD_SIZE];

    while (1) {
        uart_send_string("user@rpi3:~$ ");
        // read_line_low_power(cmd_buf, CMD_SIZE);
        read_line(cmd_buf, CMD_SIZE);
        if (!strlen(cmd_buf))  // User input nothing but Enter
            continue;
        cmd_handler(cmd_buf);
    }
}

extern unsigned long *kpgd;

int kernel_main(char *sp)
{
    // do_dtp(uart_probe);
    // el1_table_init();
    rd_init();
    // create_kernel_pgd(0, 0x40000000);
    // set_ttbr1((unsigned long)kpgd - 0xffff000000000000);
    dynamic_mem_init();
    timerPool_init();
    core_timer_enable();
    // put32(ENABLE_IRQS_1, 1 << 29); // Enable AUX interrupt
    init_wait_pool();
    init_sleepQueue();
    init_uartQueue();

    sd_init();

    init_fops_pool();
    init_vops_pool();
    init_mnttab();
    init_oftab();
    init_vnode_pool();
    init_fstab();
    init_dev_pool();

    uart_init();
    // register_filesystem("tmpfs", (unsigned long)tmpfs_setup_mount);
    // register_filesystem("procfs", (unsigned long)procfs_setup_mount);
    register_filesystem("fat32", (unsigned long)fat32_setup_mount);
    init_fat32();
    init_root_filesystem(); // must be precede than init_tmpfs()
    // init_tmpfs();


    uart_send_string("Welcome to RPI3-OS\r\n");
//

//
    init_ts_pool();
    init_mms_pool();
    init_vma_pool();
    current = new_ts();
    current->ctx.sp = (unsigned long)kmalloc(4096);
    thread_create((unsigned long)idle, 0);
    thread_create((unsigned long)shell, 0);
    // thread_create((unsigned long)test_thread_1, "CCC\r\n");
    // thread_create((unsigned long)test_thread_2, "DDD\r\n");
    thread_create((unsigned long)user_thread, 0);
    // thread_create((unsigned long)user_thread_2, 0);
delay(50000000);



    enable_irq();
    schedule();
    // shell();
    return 0; // Should never return.
}

