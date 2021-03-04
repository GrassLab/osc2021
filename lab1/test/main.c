#include "uart.h"
#include "reboot.h"
void print_welcome();

void shell(char *input) {
    int index = 0;
    char c;
    do {
        c = uart_getc();
        input[index++] = (c != '\n')? c : '\0';
        uart_send(c);
    }while(c != '\n');
    uart_puts("\r");
}


void main() {
    uart_init();    
    print_welcome();
    while(1) {
        uart_puts("#");
        char input[20];
        shell(input);

        if(!strcmp(input, "hello")) {
            uart_puts("Hello World!\n");
        }
        else if(!strcmp(input, "help")) {
            uart_puts("hello: print Hello World!\n");
            uart_puts("help: print all available commands\n");
            uart_puts("reboot: reboot system\n");
        }
        else if(!strcmp(input, "reboot")) {
            reset(100000);
            uart_puts("input c to cancel reset\n");
            shell(input);
            if(!strcmp(input, "c")) {
                cancel_reset();
            }
        }
        else {
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
}

void print_welcome(){
    char* helloworld;
    helloworld = "\n _   _      _ _         __        __         _     _ \n\
| | | | ___| | | ___    \\ \\      / /__  _ __| | __| |\n\
| |_| |/ _ \\ | |/ _ \\    \\ \\ /\\ / / _ \\| '__| |/ _` |\n\
|  _  |  __/ | | (_) |    \\ V  V / (_) | |  | | (_| |\n\
|_| |_|\\___|_|_|\\___( )    \\_/\\_/ \\___/|_|  |_|\\__,_|\n\
                    |/                               \n";
    uart_puts(helloworld);

    // uart_puts ("                                                                                      \n");
    // uart_puts ("                                                                                      \n");
    // uart_puts ("                                                    _    _,__    _                    \n");
    // uart_puts ("                                              _pgN0MM00M0MMMMM0M000MNNpgpppq_         \n");
    // uart_puts ("                                              0M^'                     `^~7M0&        \n");
    // uart_puts ("                                              &                              0        \n");
    // uart_puts ("                                             ]#                              0        \n");
    // uart_puts ("                                             ]f                              0        \n");
    // uart_puts ("                                             4f                              0        \n");
    // uart_puts ("                                             0                              ]0        \n");
    // uart_puts ("                                             #                              ]0        \n");
    // uart_puts ("                                             #                              ]0        \n");
    // uart_puts ("                                            ]&                              4#        \n");
    // uart_puts ("                                            ]#                              ##        \n");
    // uart_puts ("                                            jA                              MA        \n");
    // uart_puts ("                                            05                              0f        \n");
    // uart_puts ("                                            M                               0         \n");
    // uart_puts ("                                            0                              ]0         \n");
    // uart_puts ("                                           ]0                              4#         \n");
    // uart_puts ("                                           j0                              Bf         \n");
    // uart_puts ("                                           #0                              B!         \n");
    // uart_puts ("                                           B&        _______,pqggpgpqpM0M0N0          \n");
    // uart_puts ("                                           000NMMM0MM000MMMM~~~~~~~'^`^```B0          \n");
    // uart_puts ("                                          ]0P~                            ]#          \n");
    // uart_puts ("                                          ]#                              MP          \n");
    // uart_puts ("                                          Q&                              0f          \n");
    // uart_puts ("                                          #&                             ]06  _       \n");
    // uart_puts ("                                          0f                         _,,p0000M00      \n");
    // uart_puts ("                                          0'            __,,gggp0MM00000000~~``       \n");
    // uart_puts ("                                      _ _j0q,gggpgp0MMM000MM00000000000000&           \n");
    // uart_puts ("                              00M00M000M000MM@M~~~^    ]0     ^~MM00000000f           \n");
    // uart_puts ("    __,      ,ggg_             ~~~0F~`]0~              ]0           '70000f           \n");
    // uart_puts ("   gMM0g   qMF~^'Mp               Q   Q0               ]#              ~#0            \n");
    // uart_puts ("  #'   M0p05     '0               #   #&               4#               ]0            \n");
    // uart_puts (" gf     MM'       0               #   Q#               0M               j&            \n");
    // uart_puts (" 0      ^^       _0               #   `0_             0@             _yQMc            \n");
    // uart_puts ("M&               #H               H    '0g          p#M           _y*^ J0             \n");
    // uart_puts ("4&              j#               ]&     '0MNgggpgg00@'         _p&~]  j0`             \n");
    // uart_puts (" 0             jM'               i6      0f~~~~'~~        _,g+~ E  ] p0f              \n");
    // uart_puts (" M&           p0'                0#      #6          __yaMP`    #  jM0'               \n");
    // uart_puts ("  Bp        _g0                 N00&     '0p___,ma#V'^    6     &g00~                 \n");
    // uart_puts ("   M&     _pMP                 00000&      M0#_   f       !  __p0M~                   \n");
    // uart_puts ("    '0gpgg0~                 _0000000&       ~M0Np&______gQp0MM^                      \n");
    // uart_puts ("      ~M7^                  _000000000A          `~~MMMMF~~                           \n");
    // uart_puts ("                            '~~'~~` `                                                 \n");
    // uart_puts ("                                                                                      \n");
    // uart_puts ("                                                                                      \n");
}
