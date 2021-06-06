#include "fd.h"

void init_fd_table(struct fd_node * fd_table)
{
    for (int i = 0; i < FD_TABLE_SIZE; ++i)
    {
        fd_table[i].used = false;
        fd_table[i].f = NULL;
    }
}

int insert_fd(struct fd_node * fd_table, struct File * f)
{
    for(int i = 0; i < FD_TABLE_SIZE; ++i)
    {
        if (fd_table[i].used == false)
        {
            fd_table[i].used = true;
            fd_table[i].f = f;
            return i;
        }
    }

    uart_puts("Error: FD_TABLE full.\n");
    return -1;
}

void remove_fd(struct fd_node * fd_table, int fd)
{
    if (fd > FD_TABLE_SIZE) 
    {
        return;
    }

    fd_table[fd].used = false;
    fd_table[fd].f = NULL;
}

