#include "shell.h"
#include "string.h"
#include "util.h"
#include "uart.h"
#include "cpio.h"
#include "type.h"
#include "buddy.h"
#include "slab.h"
#include "exception.h"

void shell_welcome_message() {
    uart_puts(" _   _      _ _       \n");
    uart_puts("| | | |    | | |      \n");
    uart_puts("| |_| | ___| | | ___  \n");
    uart_puts("|  _  |/ _ \\ | |/ _ \\ \n");
    uart_puts("| | | |  __/ | | (_) |\n");
    uart_puts("\\_| |_/\\___|_|_|\\___/ \n");
    uart_puts("\n");
}

void shell_start() {
    uart_puts("$ ");

    char buffer[32];
    int i = 0;

    while(1) {
        char c = uart_getc();
        uart_send(c);
        if(c == '\n') {
            buffer[i] = 0x00;
            command_controller(buffer);
            break;
        } else {
            buffer[i] = c;
            i++;
        }
    }
}

void command_controller(char *cmd) {
    if      (!strcmp("help"            , cmd))     { command_help(); } 
    else if (!strcmp("hello"           , cmd))     { command_hello(); }
	else if (!strcmp("reboot"          , cmd))     { command_reboot(); }
    else if (!strcmp("cpio_info"       , cmd))     { command_cpio_info(); }
    else if (!strcmp("ls"              , cmd))     { command_ls(); }
    else if (!strcmp("allocate"        , cmd))     { command_allocate(); }
    else if (!strcmp("freez"           , cmd))     { command_freez(); }
    else if (!strcmp("freei"           , cmd))     { command_freei(); }
    else if (!strcmp("meminfo"         , cmd))     { command_meminfo(1); }
    else if (!strcmp("meminfo -a"      , cmd))     { command_meminfo(1); }
    else if (!strcmp("meminfo -u"      , cmd))     { command_meminfo(2); }
    else if (!strcmp("kmalloc"         , cmd))     { command_kmalloc(); }
    else if (!strcmp("kfree"           , cmd))     { command_kfree(); }
    else if (!strcmp("load_program"    , cmd))     { command_load_user_program_to_addr(); }
    else if (!strcmp("run_program"     , cmd))     { command_run_user_program(); }
    else if (!strcmp("currentel"       , cmd))     { command_get_currentel(); }
    else if (!strcmp("test"            , cmd))     { command_test(); }
    else    { command_not_found(); }
}

void command_help() {
    uart_puts("========================================\n");
    uart_puts("Usage\n");
    uart_puts("  help\t\t:\tprint all available commands.\n");
    uart_puts("  hello\t\t:\tprint Hello World!\n");
    uart_puts("  reboot\t:\treboot ths system.\n");
    uart_puts("  cpio_info\t:\tshow cpio info.\n");
    uart_puts("  ls\t\t:\tshow cpio list.\n");
    uart_puts("  allocate\t:\tallocate memory.\n");
    uart_puts("  freez\t\t:\tfree memory by size.\n");
    uart_puts("  freei\t\t:\tfree memory by index.\n");
    uart_puts("  meminfo\t:\tlist memory info.\n");
    uart_puts("  kmalloc\t:\tmalloc memory for kernel.\n");
    uart_puts("  kfree\t:\tfree memory for kernel.\n");
    uart_puts("  test\t\t:\ttesting kmalloc and kfree.\n");
    uart_puts("  load_program\t:\tload user program to specific address.\n");
    uart_puts("  run_program\t:\trun user program at specific address.\n");
    uart_puts("  currentel\t:\tget current execption level.\n");
    uart_puts("========================================\n");
}

void command_hello() {
    uart_puts("Hello world\n");
}

void command_reboot() {
    uart_puts("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;
    
	while(1);
}


void command_cpio_info() {
    struct cpio_info info;
    
    uart_puts("CPIO_ARCHIVE_LOCATION: 0x");
    uart_puti((uint64_t)CPIO_ARCHIVE_LOCATION, 16);
    uart_puts("\n");


    if(cpio_info((uint64_t *)CPIO_ARCHIVE_LOCATION, &info)) {
        uart_puts("Error on cpio_info command\n");
        return;
    }

    uart_puts("file count: ");
    uart_puti(info.file_count, 10);
    uart_puts("\n");

    uart_puts("maximum size of a file name: ");
    uart_puti(info.max_path_sz, 10);
    uart_puts("\n");
}

void command_ls() {
    struct cpio_info info;

    if(cpio_info((uint64_t *)CPIO_ARCHIVE_LOCATION, &info)) {
        uart_puts("Error on cpio_info command\n");
        return;
    }

    char ls_buffer[100][100];
    unsigned long buf_len = 100;

    for(int i = 0; i < 100; i++)
        for(int j = 0; j < 100; j++)
            ls_buffer[i][j] = 0;

    cpio_ls((uint64_t *)CPIO_ARCHIVE_LOCATION, ls_buffer, buf_len);

    for(int i = 0; i < info.file_count; i++) {
        uart_puti(i, 10);
        uart_puts(": ");
        uart_puts(ls_buffer[i]);
        uart_puts("\n");
    }
}

void command_allocate() {
    char input_buffer[32] = { 0 };

    uart_puts("Enter size to allocate (in KB): ");

    int i = 0;
    while(1) {
        char c = uart_getc();
        uart_send(c);
        if(c == '\n') {
            input_buffer[i] = 0x00;
            break;
        } else {
            input_buffer[i] = c;
            i++;
        }
    }

    int size_in_kbyte = atoi(input_buffer);
    
    uart_puts("Allocating memory: ");
    uart_puts(input_buffer);
    uart_puts(" KB\n");

    struct buddy_frame *allocated = allocate_frame(size_in_kbyte);

    if(allocated == NULL) {
        uart_puts("Fail: Out of memory !\n");
        return ;
    }
    
    uart_puts("Allocated frame's index: ");
    uart_puti(allocated->index, 10);
    uart_puts("\n");

    uart_puts("Allocated in 0x");
    uart_puti((uint64_t)allocated->start_address, 16);
    uart_puts("\n");
    
}

void command_freez() {
    char input_buffer[32] = { 0 };

    uart_puts("Enter size to free (in KB): ");

    int i = 0;
    while(1) {
        char c = uart_getc();
        uart_send(c);
        if(c == '\n') {
            input_buffer[i] = 0x00;
            break;
        } else {
            input_buffer[i] = c;
            i++;
        }
    }

    int size_in_kbyte = atoi(input_buffer);

    uart_puts("Free memory: ");
    uart_puts(input_buffer);
    uart_puts(" KB\n");

    int success = free_frame_by_size(size_in_kbyte);

    if(success == -1) {
        uart_puts("Fail: No such frame to release !\n");
        return ;
    }


}

void command_freei() {
    char input_buffer[32] = { 0 };

    uart_puts("Enter index to free: ");

    int i = 0;
    while(1) {
        char c = uart_getc();
        uart_send(c);
        if(c == '\n') {
            input_buffer[i] = 0x00;
            break;
        } else {
            input_buffer[i] = c;
            i++;
        }
    }

    int index_to_free = atoi(input_buffer);
    int success = free_frame_by_index(index_to_free);

    if(success != 0)
        uart_puts("Fail: No such frame to release !\n");
}

void command_meminfo(int mode) {
    if(mode == 2)
        print_used_memory_with_uart();
    else
        print_available_memory_with_uart();
}

void command_kmalloc() {
    char input_buffer[32] = { 0 };

    uart_puts("Enter size in bytes to kmalloc: ");

    int i = 0;
    while(1) {
        char c = uart_getc();
        uart_send(c);
        if(c == '\n') {
            input_buffer[i] = 0x00;
            break;
        } else {
            input_buffer[i] = c;
            i++;
        }
    }

    int size_to_allocate = atoi(input_buffer);
    uint64_t *adr = kmalloc(size_to_allocate);

    uart_puts("allocated at: 0x");
    uart_puti((uint64_t)adr, 16);
    uart_puts("\n");
}

void command_kfree() {
    char input_buffer[32] = { 0 };

    uart_puts("Enter address in hex (without 0x) to kfree: ");

    int i = 0;
    while(1) {
        char c = uart_getc();
        uart_send(c);
        if(c == '\n') {
            input_buffer[i] = 0x00;
            break;
        } else {
            input_buffer[i] = c;
            i++;
        }
    }

    int address_to_free = hextoi(input_buffer);
    
    int res = kfree(address_to_free);
    if(res == 0)
        uart_puts("[debug] successfully freed memory.\n");
    else
        uart_puts("[debug] error!\n");

}

void command_load_user_program_to_addr() {
    char input_buffer[32] = { 0 };

    uart_puts("Enter file name to load: ");

    int i = 0;
    while(1) {
        char c = uart_getc();
        uart_send(c);
        if(c == '\n') {
            input_buffer[i] = 0x00;
            break;
        } else {
            input_buffer[i] = c;
            i++;
        }
    }

    input_buffer[31] = 0x00;


    // Search file in cpio archive
    uint64_t current_ptr = CPIO_ARCHIVE_LOCATION;

    while(1) {
        struct cpio_header *cpio_ptr = (struct cpio_header *)current_ptr;

        int name_size = hextoint64(cpio_ptr->c_namesize);
        int file_size = hextoint64(cpio_ptr->c_filesize);

        current_ptr = current_ptr + sizeof(struct cpio_header);

        char *file_name = (char *)current_ptr;

        if(!strcmp(file_name, "TRAILER!!!"))
            break;

        current_ptr = current_ptr + name_size;
        char *file_content = (char *)current_ptr;

        current_ptr = current_ptr + file_size;

        if(!strcmp(file_name, input_buffer)) {
            unsigned char *target_address = (unsigned char *)0x20800000;

            while(file_content != (char *)current_ptr) {
                *target_address = *file_content;
                file_content += 1;
                target_address += 1;
            }

            uart_puts("[debug] user program load completed\n");

            break;
        }
    }
}

void command_run_user_program() {
    uart_puts("[debug] running user program at 0x20800000\n");

    // set spsr_el1 to 0x3c0
    asm volatile("mov x0, 0x3c0;"
                 "msr spsr_el1, x0;"); 

    // set elr_el1 to user program's address
    asm volatile("mov x0, 0x20800000;"
                 "msr elr_el1, x0;"); 

    // set sp_el0 to user program's stack address
    asm volatile("mov x0, 0x20800000;"
                 "msr sp_el0, x0;"); 

    asm volatile("eret"); 

    uart_puts("[debug] user program execute completed\n");
}

void command_get_currentel() {
    unsigned long currentel = get_current_exception_level();
    uart_puts("currentel: ");
    uart_puti(currentel, 10);
    uart_puts("\n");
}

void command_test() {
    print_memory_with_uart((uint64_t *)CPIO_ARCHIVE_LOCATION, 64);
}

void command_not_found() {
    uart_puts("command not found\n");
}