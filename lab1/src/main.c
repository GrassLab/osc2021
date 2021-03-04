#include "shell.h"

#define CMD_LEN 128

enum shell_flag{
    Read,
    Execute
};

int main(){
    shell_init();
    enum shell_flag flag = Read;
    while(1){
        char cmd[CMD_LEN];
        switch (flag) {
	    case Read:
		shell_input(cmd);
		flag = Execute;
		break;
	    case Execute:
		shell_controller(cmd);
		flag = Read;
		break;
	}
    }
}
