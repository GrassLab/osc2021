#include "uart.h"
#include "string.h"

void shell(char *input) {
    int index = 0;
    char c;
    do {
        c = uart_getc();
        input[index++] = (c != '\n')? c : '\0';
        uart_send(c);
    }while(c != '\n');
    uart_send('\r');
}

int atoi(char* s, int base) {
    int i = 0;
    int num = 0;
    int negative = 0;

    while(s[i] != '\0') {
        if(i == 0 && s[i] == '-') {
            negative = 1;
        }
        if(base == 10) {
            int digit = s[i] - '0';
            num = num*10 + digit;
        }
        if(base == 16) {
            int digit = s[i] >= 'A' ? s[i] - 'A'+ 10 : s[i] - '0';
            num = num*16 + digit;
        }
        i++;
    }
    
    if(negative) return num *= -1;
    return num;
}


void itoa(int num, char *s) {
    int i = 0;
    if(num == 0)
        s[i++] = '0';
    while(num != 0) {
        int digit = num % 10;
        s[i++] = digit + '0';
        num /= 10;
    }
    s[i] = '\0';
    
    // reverse s
    for(int j = i-1; j >= i/2; j--) {
        char temp = s[j];
        s[j] = s[i-1-j];
        s[i-1-j] = temp;
    }
}

// int strcmp(char* a,char* b){
// 	while(*a){
// 		if(*a!=*b)return 1;
// 		a++;
// 		b++;
// 	}
// 	if(*a!=*b)return 1;
// 	return 0;
// }

int ls(char input[20], int readcontent)
{
    volatile unsigned char *kernel = (unsigned char *) 0x8000000;
    //volatile unsigned char *kernel = (unsigned char *) 0x20000000;
    volatile unsigned char *filename;
    int file_size;
    int name_size;
    int file_size_offset    = 6+8+8+8+8+8+8;
    int name_size_offset    = 6+8+8+8+8+8+8+8+8+8+8+8;

    while(1){ 
        file_size = 0;
        name_size = 0;
        
        for(int i=0;i<8;i++){
            if(kernel[file_size_offset + i] >= 'A' && kernel[file_size_offset + i] <= 'F')
                file_size = file_size * 0x10 + ((int)kernel[file_size_offset + i]) - 'A' + 0xA;            
            else
                file_size = file_size * 0x10 + ((int)kernel[file_size_offset + i]) - '0' ;

            if(kernel[name_size_offset + i] >= 'A' && kernel[name_size_offset + i] <= 'F')
                name_size = name_size * 0x10 + ((int)kernel[name_size_offset + i]) - 'A' + 0xA;
            else
                name_size = name_size * 0x10 + ((int)kernel[name_size_offset + i]) - '0' ;         
        }

        name_size += 0x6E;

        if((file_size % 4) != 0)
            file_size += (4 - (file_size % 4));

        if((name_size % 4) != 0)
            name_size += (4 - (name_size % 4));

        filename = (unsigned char *) kernel + 0x6E;

        if(!strcmp(kernel + 0x6E, "TRAILER!!!"))
            return 0;

        if(strcmp(kernel + 0x6E, ".") && !readcontent){
            uart_puts(filename);
            uart_puts("\n");
        }
        if(!strcmp(kernel + 0x6E, input) && readcontent){
            for(int i=0; i<file_size; i++){
                uart_send(*(kernel + name_size + i));
                if(!strcmp(*(kernel + name_size + i),'\n')) uart_send('\r');
            }                
            return 1;
        }

        kernel += file_size + name_size ;
    }

}

void print_regs(unsigned long regs, int index){
    if(index == 0)
        uart_puts("spsr_el1: ");
    else if(index == 1)
        uart_puts("elr_el1:  ");
    else if(index == 2)
        uart_puts("esr_el1:  ");
    uart_puts("0x");
    uart_int(regs);
    uart_puts("\n");
}

void print_timer(unsigned long clock, unsigned long freq){
    uart_puts("current time: ");
    uart_puts("0x");
    unsigned long time = clock/freq;
    uart_int(time);
    uart_puts("\n");
}

void print_welcome(int index){
    char* helloworld;
    char* boot;
    helloworld = "\n _   _      _ _         __        __         _     _ \n\
| | | | ___| | | ___    \\ \\      / /__  _ __| | __| |\n\
| |_| |/ _ \\ | |/ _ \\    \\ \\ /\\ / / _ \\| '__| |/ _` |\n\
|  _  |  __/ | | (_) |    \\ V  V / (_) | |  | | (_| |\n\
|_| |_|\\___|_|_|\\___( )    \\_/\\_/ \\___/|_|  |_|\\__,_|\n\
                    |/                               \n";
    boot = "\n\
 _                     _ _               ________  ________ \n\
| |                   | (_)             |_   _|  \\/  |  __ \\\n\
| |     ___   __ _  __| |_ _ __   __ _    | | | .  . | |  \\/\n\
| |    / _ \\ / _` |/ _` | | '_ \\ / _` |   | | | |\\/| | | __ \n\
| |___| (_) | (_| | (_| | | | | | (_| |  _| |_| |  | | |_\\ \\\n\
\\_____/\\___/ \\__,_|\\__,_|_|_| |_|\\__, |  \\___/\\_|  |_/\\____/\n\
                                  __/ |                     \n\
                                 |___/                      \n\
    \n";
    
    if(index == 0)
        uart_puts(boot);
    else if(index == 1)
        uart_puts(helloworld);
    else{
    uart_puts ("                                                                                      \n");
    uart_puts ("                                                                                      \n");
    uart_puts ("                                                    _    _,__    _                    \n");
    uart_puts ("                                              _pgN0MM00M0MMMMM0M000MNNpgpppq_         \n");
    uart_puts ("                                              0M^'                     `^~7M0&        \n");
    uart_puts ("                                              &                              0        \n");
    uart_puts ("                                             ]#                              0        \n");
    uart_puts ("                                             ]f                              0        \n");
    uart_puts ("                                             4f                              0        \n");
    uart_puts ("                                             0                              ]0        \n");
    uart_puts ("                                             #                              ]0        \n");
    uart_puts ("                                             #                              ]0        \n");
    uart_puts ("                                            ]&                              4#        \n");
    uart_puts ("                                            ]#                              ##        \n");
    uart_puts ("                                            jA                              MA        \n");
    uart_puts ("                                            05                              0f        \n");
    uart_puts ("                                            M                               0         \n");
    uart_puts ("                                            0                              ]0         \n");
    uart_puts ("                                           ]0                              4#         \n");
    uart_puts ("                                           j0                              Bf         \n");
    uart_puts ("                                           #0                              B!         \n");
    uart_puts ("                                           B&        _______,pqggpgpqpM0M0N0          \n");
    uart_puts ("                                           000NMMM0MM000MMMM~~~~~~~'^`^```B0          \n");
    uart_puts ("                                          ]0P~                            ]#          \n");
    uart_puts ("                                          ]#                              MP          \n");
    uart_puts ("                                          Q&                              0f          \n");
    uart_puts ("                                          #&                             ]06  _       \n");
    uart_puts ("                                          0f                         _,,p0000M00      \n");
    uart_puts ("                                          0'            __,,gggp0MM00000000~~``       \n");
    uart_puts ("                                      _ _j0q,gggpgp0MMM000MM00000000000000&           \n");
    uart_puts ("                              00M00M000M000MM@M~~~^    ]0     ^~MM00000000f           \n");
    uart_puts ("    __,      ,ggg_             ~~~0F~`]0~              ]0           '70000f           \n");
    uart_puts ("   gMM0g   qMF~^'Mp               Q   Q0               ]#              ~#0            \n");
    uart_puts ("  #'   M0p05     '0               #   #&               4#               ]0            \n");
    uart_puts (" gf     MM'       0               #   Q#               0M               j&            \n");
    uart_puts (" 0      ^^       _0               #   `0_             0@             _yQMc            \n");
    uart_puts ("M&               #H               H    '0g          p#M           _y*^ J0             \n");
    uart_puts ("4&              j#               ]&     '0MNgggpgg00@'         _p&~]  j0`             \n");
    uart_puts (" 0             jM'               i6      0f~~~~'~~        _,g+~ E  ] p0f              \n");
    uart_puts (" M&           p0'                0#      #6          __yaMP`    #  jM0'               \n");
    uart_puts ("  Bp        _g0                 N00&     '0p___,ma#V'^    6     &g00~                 \n");
    uart_puts ("   M&     _pMP                 00000&      M0#_   f       !  __p0M~                   \n");
    uart_puts ("    '0gpgg0~                 _0000000&       ~M0Np&______gQp0MM^                      \n");
    uart_puts ("      ~M7^                  _000000000A          `~~MMMMF~~                           \n");
    uart_puts ("                            '~~'~~` `                                                 \n");
    uart_puts ("                                                                                      \n");
    uart_puts ("                                                                                      \n");
    }
}