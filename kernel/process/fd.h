#ifndef FD_H
#define FD_H

#include "../fs/vfs.h"

#define FD_TABLE_SIZE 100

struct fd_node{
    int used;
    struct File * f;
};

void init_fd_table(struct fd_node * fd_table);
int insert_fd(struct fd_node * fd_table, struct File * f);
void remove_fd(struct fd_node * fd_table, int fd);

#endif