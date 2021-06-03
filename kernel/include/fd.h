#ifndef _FD_H
#define _FD_H

#define MAX_FD_NUM (10)

#include "vfs.h"

struct node {
    int fd_number;
    struct node* next;
};

struct fd_table {
    struct file *list[10];

    // unused fd number
    struct node *stock_head; 
};

void init_fd_table(struct fd_table *fd_table);
struct node *newNode(int fd_number);
int pop_stock(struct node** head);
void push_stock(struct node **head, int fd_number);
int insert_fd(struct fd_table *table, struct file *file);
void remove_fd(struct fd_table *table, int fd);

#endif