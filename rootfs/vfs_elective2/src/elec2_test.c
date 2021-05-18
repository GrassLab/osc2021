#include "printf.h"
#include "sys.h"
#include "uart.h"

#define O_CREAT 1
int main(void) {
    init_printf(0, putc);
    
    printf("--------------> Lab6 eletive2 | vfs_elective2_user_process_test()<--------------\n");
    char buf[8];
    call_sys_mkdir("mnt");
    int fd = call_sys_open("/mnt/a.txt", O_CREAT);
    call_sys_write(fd, "Hi", 2);
    call_sys_close(fd);
    call_sys_chdir("mnt");
    fd = call_sys_open("./a.txt", 0);
    if (fd < 0) {
        printf("Open ./a.txt Error!\n");
    }
    int sz = call_sys_read(fd, buf, 2);
    printf("size = %d, content = %s\n", sz, buf);

    call_sys_chdir("..");
    call_sys_mount("tmpfs", "mnt", "tmpfs");
    fd = call_sys_open("mnt/a.txt", 0);
    if (fd >= 0) {
        printf("Error, mnt/a.txt should not exist!");
    }
    
    call_sys_chdir("/mnt");
    call_sys_chdir("..");

    call_sys_unmount("/mnt");
    fd = call_sys_open("/mnt/a.txt", 0);
    if (fd < 0) {
        printf("Open /mnt/a.txt Error!\n");
    }
    sz = call_sys_read(fd, buf, 2);
    printf("size = %d, content = %s\n", sz, buf);

    call_sys_exit();

    return 0;
}