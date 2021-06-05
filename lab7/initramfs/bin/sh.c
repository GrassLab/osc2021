#include <stdlib.h>
#include <printf.h>
#include <string.h>
#define NEWLINE '\r'
#define assert(b)                                              \
    if (!(b)) {                                                \
        printf("test failed: %s, line %d\r\n", #b, __LINE__);  \
        return;                                                \
    }                                                          \

void test1(void) {
    char buf[100];
    int a = open("/././hello", O_CREAT);
    int b = open("/../.././world", O_CREAT);
    write(a, "Hello ", 6);
    write(b, "World!", 6);
    close(a);
    close(b);
    b = open("/hello", 0);
    a = open("/world", 0);
    int sz;
    sz = read(b, buf, 100);
    sz += read(a, buf + sz, 100);
    buf[sz] = '\0';
    printf("size: %d, s: %s\r\n", sz, buf);
    close(a);
    close(b);
}

void test2() {
  char buf[8];
  mkdir("mnt");
  int fd = open("/mnt/a.txt", O_CREAT);
  write(fd, "Hi", 2);
  close(fd);
  chdir("mnt");
  fd = open("./a.txt", 0);
  assert(fd >= 0);
  read(fd, buf, 2);
  assert(strncmp(buf, "Hi", 2) == 0);

  chdir("..");
  mount("tmpfs", "mnt", "tmpfs");
  fd = open("mnt/a.txt", 0);
  assert(fd < 0);

  umount("/mnt");
  fd = open("/mnt/a.txt", 0);
  assert(fd >= 0);
  read(fd, buf, 2);
  assert(strncmp(buf, "Hi", 2) == 0);
}

int split(char *buf, char *outbuf[], int n) {
    char *ps, *pe;
    int idx = 0;
    ps = pe = buf;

    while(idx < n) {
        while (*pe && *pe != ' ') pe++;

        int size = pe - ps;
        if (size) {
            outbuf[idx++] = ps;
        }

        if (*pe) {
            *pe = '\0';
            pe++;
            while (*pe == ' ') pe++;
            ps = pe;
        } else {
            break;
        }
    }

    return idx;
}

int interact_readline_uart(char *buffer) {
    char c = '\0';
    int count = 0;
    while (c != NEWLINE) {
        uart_read(&c, 1);

        buffer[count++] = c;
        if (c != NEWLINE) {
            uart_write(&c, 1);
        } else {
            uart_write("\r\n", 2);
        }
    }

    buffer[count-1] = '\0';
    return count;
}

char buf[0x100];
char pwd[0x30];
char *args[5];

int main() {
    puts("[Shell] Enjoy your root shell :)");
    mkdir("sdcard");
    mount("fat32", "sdcard", "fat32");
    while (1) {
        getcwd(pwd, sizeof(pwd));
        printf("root:%s # ", pwd);
        interact_readline_uart(buf);

        if (!strncmp(buf, "ls", 2)) {
            if (split(buf, args, 2) == 2) {
                args[2] = NULL;
                if (!fork()) {
                    exec("/bin/ls", args);
                }
                /* TODO: waitpid */
                sleep(500);
            }
        }
        else if (!strncmp(buf, "mkdir", 5)) {
            if (split(buf, args, 2) == 2) {
                mkdir(args[1]);
            }
        }
        else if (!strncmp(buf, "rmdir", 5)) {
            if (split(buf, args, 2) == 2) {
                rmdir(args[1]);
            }
        }
        else if (!strncmp(buf, "cd", 2)) {
            if (split(buf, args, 2) == 2) {
                chdir(args[1]);
            }
        }
        else if (!strncmp(buf, "mount", 5)) {
            if (split(buf, args, 3) == 3) {
                mount(args[1], args[2], args[1]);
            }
        }
        else if (!strncmp(buf, "umount", 6)) {
            if (split(buf, args, 2) == 2) {
                umount(args[1]);
            }
        }
        else if (!strcmp(buf, "test1")) {
            test1();
        }
        else if (!strcmp(buf, "test2")) {
            test2();
        }
    }
}