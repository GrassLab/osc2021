#include "fd.h"
#include "dynamic_alloc.h"
#include "vfs.h"

#include "io.h"

void init_fd_table(struct fd_table *fd_table)
{
    int i;

    struct node *fd_node, *prev_node = NULL;
    for (i = 0; i < MAX_FD_NUM; i++){
        fd_node = newNode(i);

        if (prev_node) {
            prev_node->next = fd_node;
        } else {
            fd_table->stock_head = fd_node;
        }

        prev_node = fd_node;
    }
}

struct node *newNode(int fd_number)
{
    struct node* temp = malloc(sizeof(struct node));
    temp->fd_number = fd_number;
    temp->next = NULL;
 
    return temp;
}

int pop_stock(struct node** head)
{
    struct node* temp = *head;
    (*head) = (*head)->next;

    int fd_num = temp->fd_number;
    free(temp);

    return fd_num;
}

void push_stock(struct node **head, int fd_number)
{
    struct node* start = (*head);
 
    // Create new Node
    struct node* temp = newNode(fd_number);
 
    if ((*head)->fd_number > fd_number) {
        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else {
 
        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
            start->next->fd_number < fd_number) {
            start = start->next;
        }
 
        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

int insert_fd(struct fd_table *table, struct file *f_addr)
{
    int first_fd_num = pop_stock(&(table->stock_head));
    table->list[first_fd_num] = f_addr;

    return first_fd_num;
}

void remove_fd(struct fd_table *table, int fd)
{
    struct file *f = table->list[fd];

    free(f);

    push_stock(&table->stock_head, fd);
}