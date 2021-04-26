void shell(char *input);
void itoa(int num, char *s);
int atoi(char* s, int base);
int ls(char input[20], int readcontent);
void print_welcome(int index);
void dec_hex(unsigned long number, char *hexadecimal);
void boot_from_initramfs(char* path,unsigned long a_addr,char** argv,unsigned long* task_a_addr,unsigned long* task_a_size);
unsigned long argvPut(char** argv,unsigned long ret);