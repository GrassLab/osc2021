#pragma once

#include <types.h>
#include <kernel/thread.h>
#include <files.h>

struct file_entry {
    union {
        char filename[24];
        uint32_t next_free_entry;
    };
    
    uint32_t block;
    uint32_t file_size;
};

struct fd_entry {
    union {
        file_entry *entry;
        int64_t next_free_fd;
    };
    uint32_t pos;
    uint32_t mode;
};

int open(char *filename, int mode);
size_t read(int fd, char *buf, size_t count);
size_t write(int fd, char *buf, size_t count);
void close(int fd);
