#include "shell.h"
#include "mm.h"
#define CMD_LEN 128

enum shell_status {
    Read,
    Parse
};

int main() {
    shell_init();
    
    
    //void* addr1 = kmalloc(8888);
    //void* addr2 = kmalloc(4096);
    //void* addr3 = kmalloc(65);
    //void* addr4 = kmalloc(31);
    //void* addr5 = kmalloc(64);
    //uart_printf("%x +++++\n",addr1);
    //uart_printf("%x +++++\n",addr2);
    //uart_printf("next\n");
    //kfree(addr1);
 
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
