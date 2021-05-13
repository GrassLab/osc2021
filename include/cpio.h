#ifndef CPIO_H
#define CPIO_H

#define PROCESS_SIZE (1<<12)

typedef struct file_property {
    char *fname;
    unsigned char *fdata;
    unsigned long fmode;
    int nsize;
    int fsize;
} f_prop;

void getFileData(char *target);
void list_file();
void load_user_program();
unsigned long load_user_program_withArgv(char *name, char **argv);
void *get_cpio_base();
f_prop* get_file_property(void* addr);

#endif