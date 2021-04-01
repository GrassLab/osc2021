#include "shell.h"
#include "mm.h"
#define CMD_LEN 128

enum shell_status {
    Read,
    Parse
};

int main() {
    shell_init();
    
    
    void* addr1 = kmalloc(8888);
    uart_printf("###################");
    void* addr2 = kmalloc(8888);
    void* addr3 = kmalloc(66);
    //noid* addr4 = kmalloc(31);
    
    uart_printf("%x +++++\n",addr1);
    uart_printf("%x +++++\n",addr2);
    uart_printf("%x +++++\n",addr3);
    kfree(addr3);
    void* addr5 = kmalloc(67);
    uart_printf("%x +++++\n",addr5);
    void* addr6 = kmalloc(68);
    uart_printf("%x +++++\n",addr6);
    uart_printf("next\n");
    kfree(addr1);
    kfree(addr2);
    kfree(addr5);
    kfree(addr6);
    enum shell_status status = Read;
    while (1) {
        char cmd[CMD_LEN];
        switch (status) {
            case Read:
                shell_input(cmd);
                status = Parse;
                break;

            case Parse:
                shell_controller(cmd);
                status = Read;
                break;
        }
    }
}
