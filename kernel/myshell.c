#include "../include/uart.h"
#include "../include/cpio.h"
#include "../include/string.h"
#include "../include/mm.h"

struct cpio_header_info {
    const char *filename;
    unsigned long filesize;
    void *data;
    struct cpio_header *next;
};

static char* cpio_strcpy(char *to, const char *from) {
    char *save = to;
    while (*from != 0) {
        *to = *from;
        to++;
        from++;
    }
    return save;
}

void command(char buf[]){

    if(!strcmp(buf,"help")) command_help();
    else if(!strcmp(buf, "hello")) command_hello();
    else if(!strcmp(buf, "reboot")) command_reboot();
    else if(!strcmp(buf, "cpio")) command_cpio();
    else if(!strcmp(buf, "cpio-ls")) command_cpiols();
    else if(!strcmp(buf, "mm")) command_mm();
    else if(strlen(buf)>5) command_cpiofile(buf);
    else command_notfound();
}

void command_help(){

    uart_puts("\n========================================\n");
    uart_puts("Usage\t:\t<command> \n");
    uart_puts("help\t:\tprint all available commands.\n");
    uart_puts("hello\t:\tprint Hello World!\n");
    uart_puts("reboot\t:\treboot raspi3.\n");
    uart_puts("cpio\t:\tparse cpio archive.\n");
    uart_puts("cpio-ls\t:\tlist files.\n");
    uart_puts("cpio-<file>\t:\tget the content of files.\n");
    uart_puts("mm\t:\tmemory management.\n");
    uart_puts("\n========================================\n");
}

void command_hello(){

    uart_puts("Hello World!\n");

}

void command_reboot(){

    uart_puts("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;

}

void command_notfound(){

    uart_puts("Command not found.\t");
    uart_puts("Try <help>\n");

}

void command_cpio(){

    struct cpio_info info;
    char output_buffer[50] = { 0 };
    uart_send('\r');

    // uart_puts("CPIO_ARCHIVE_LOCATION: ");
    // itoa(CPIO_ARCHIVE_LOCATION, output_buffer, 10);
    // uart_puts(output_buffer);
    // uart_send('\n');
    // uart_send('\r');

    if(cpio_info(CPIO_ARCHIVE_LOCATION, &info)){
        uart_puts("cpio_info error.\r\n");
        return -1;
    }

    uart_puts("Number of files : ");
    itoa(info.file_count, output_buffer, 10);
    uart_puts(output_buffer);
    uart_send('\n');
    uart_send('\r');
    uart_puts("Maximum of filename : ");
    itoa(info.max_path_sz, output_buffer, 10);
    uart_puts(output_buffer);

}

void command_cpiols(){

    struct cpio_info info;
    char buffer[100][100];
    int len = 100;
    uart_send('\r');

    if(cpio_info(CPIO_ARCHIVE_LOCATION, &info)){
        uart_puts("cpio_info error.\r\n");
        return -1;
    }

    for(int i = 0; i < 100; i++)
        for(int j = 0; j < 100; j++)
            buffer[i][j] = 0;

    uart_puts("\r");
    cpio_ls((void*)CPIO_ARCHIVE_LOCATION, buffer, len);
    for(int i = 0; i < info.file_count; i++) {
        uart_puts(buffer[i]);
        uart_puts("\n");
    }
}

void command_cpiofile(char buf[]){
    
    char file[20]={0}, content[100]={0};
    struct cpio_info info;

    cpio_strcpy(file, buf+5);
    uart_send('\r');
    // uart_puts(file);
    // uart_send('\n');
    // uart_send('\r');
    cpio_get_file(CPIO_ARCHIVE_LOCATION, file, content);
    uart_puts("File Content : ");
    uart_puts(content);
    uart_send('\r');

}

void command_mm(){

    memory_init();
    struct page *alloc_1 = page_alloc(0);
    struct page *alloc_2 = page_alloc(1);
    struct page *alloc_3 = page_alloc(0);
    
    page_free(alloc_2);
    page_free(alloc_1);
    page_free(alloc_3);

    unsigned long *adr[32];
    for(int i=0;i<32;i++){
        unsigned long *addr = kmalloc(128);
        adr[i] = addr;
        uart_hex(addr);
        uart_puts("\n");
    }
    
    unsigned long *addr = kmalloc(128);
    uart_hex(addr);
    uart_puts("\n");

    for(int i=0;i<32;i++){
        kfree(adr[i]);
    }
    kfree(addr);
    
    // addr2 = kmalloc(64);
    // uart_hex(addr2);
    // uart_puts("\n");

    // addr3 = kmalloc(63);
    // uart_hex(addr3);
    // uart_puts("\n");
    
    // addr4 = kmalloc(2049);
    // uart_hex(addr4);
    // uart_puts("\n");

    // addr5 = kmalloc(2049);
    // uart_hex(addr5);
    // uart_puts("\n");

    // kfree(addr1);
    // kfree(addr2);
    // kfree(addr3);
    // kfree(addr4);
    // kfree(addr5);

}

void my_shell(){

    uart_send('\n');
    uart_send('\r');
    uart_puts("$ ");

    char buf[32];
    int count = 0;

    while(1){

        /* get one-by-one input */
        /* until recv '\n' for finishing command */
        char c = uart_getc();
        uart_send(c);
        if(c == '\n'){
            buf[count] = '\0';
            command(buf);
            for(int i=0;i<count;i++)
                buf[i] = "";
            count = 0;
            uart_send('\n');
            uart_send('\r');
            uart_puts("$ ");
        }
        else{
            buf[count] = c;
            count++;
        }    
    }

}