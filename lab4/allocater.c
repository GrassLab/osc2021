#include "uart.h"
#include "allocater.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned long int LOW_MEM = 0x00000000 ;
static unsigned long int HIGH_MEM = 0x01000000 ;
static int PAGE_SIZE = 4096;
//static unsigned long int TOTAL_PAGE_NUM  = ((HIGH_MEM - LOW_MEM) / PAGE_SIZE);
static int POWER = 12;

static unsigned long int header[13];

typedef struct node {
    struct node *next;
} Node;

void print_int(int t)
{
    char c[20];
    if(t == 0)
    {
        uart_puts("0");
        return ;
    }
    else
    {
        int index = 0;
        while( t > 0)
        {
            c[index] = '0' + (t % 10);
            t = t / 10;
            index = index + 1;
        }
        while(index--)
        {
            uart_send(c[index]);
        }
    }
}

void print_int_alig(int t,int y)
{
    char c[20];
    if(t == 0)
    {
        for(int i=0;i<y;i++)
        {
            uart_puts("0");
        }
        return ;
    }
    else
    {
        int index = 0;
        while( t > 0)
        {
            c[index] = '0' + (t % 10);
            t = t / 10;
            index = index + 1;
        }
        for(int i=0;i<(y-index);i++)
        {
            uart_puts("0");
        }
        while(index--)
        {
            uart_send(c[index]);
        }
    }
}

void print_mem(unsigned long int t)
{
    if(t==0)
    {
        uart_puts("0x0000000000000000");
        return;
    }
    else
    {
        char c[50];
        int index = 0;
        while(t>0)
        {   
            if((t % 16 ) > 9)
            {
                c[index] = 'A' + (t % 16 -10);
            }
            else
            {
                c[index] = '0' + (t % 16); 
            }
            index = index +1;
            t = t / 16;
        }
        uart_puts("0x");
        for(int i=0 ;i<(16-index);i++)
        {
            uart_send('0');
        }
        while(index--)
        {
            uart_send(c[index]);
        }
    }
}

int my_pow(int a,int b)
{
    int c = 1;
    for(int i=0;i<b;i++)
    {
        c = c * a;
    }
    return c;
}

int my_quar(int a,int b)
{
    int c = 1,d=0;
    while(c < b)
    {
        c = c * a;
        d = d + 1;
    }
    return d;
}

void lookuppage()
{
    unsigned long int tmp;
    for(int i=0;i<(POWER + 1);i++)
    {
        print_int_alig(i,2);
        uart_puts("  : ");
        tmp = header[i];

        if(tmp != -1)
        {
            
            Node *p = tmp;
            if(p == NULL)
            {
                uart_puts("0x0000000000000000 -> ");
                p = p -> next;

            }
            while(p!=NULL)
            {
                //uart_puts("/");
                tmp = p;
                p = p -> next;
                print_mem(tmp);
                uart_puts(" -> ");
                //uart_puts("\\");
            }
        }
        uart_puts("NULL\n");
    }
}

void init_header()
{
    Node *tmp = LOW_MEM;
    tmp -> next = NULL;
    for(int i=0;i<POWER;i++)
    {
        header[i] = -1; 
    }
    header[POWER] = tmp;

    //lookuppage();
}

unsigned long int require_buddy_mem(int t)
{
    int index = 0;
    while(index <= POWER)
    {
        unsigned long int addr = header[index];
        if(index == t && header[index] != -1)
        {
            Node *tmp = header[index];
            if(tmp -> next != NULL)
            {
                tmp = tmp -> next;
                header[index] = tmp ;
            }
            else
            {
                header[index] = -1;
            }

            return addr;
        }
        if(index > t && header[index] != -1)
        {
            unsigned long int addr = header[index];
            unsigned long int next_addr = addr;
            unsigned long int head; 
            Node *tmp = header[index];
            if(tmp -> next != NULL)
            {
                header[index] = tmp -> next;
            }
            else
            {
                header[index] = -1;
            }
            while(index != t)
            {
                // print_mem(next_addr);
                // uart_puts("\n");
                //uart_puts("1\n");
                Node *p =  next_addr;
                //uart_puts("2\n");
                p -> next = NULL;
                //uart_puts("3\n");
                header[index-1] = p; 
                //uart_puts("4\n");
                next_addr = next_addr + PAGE_SIZE * my_pow(2,index-1);
                index = index - 1 ;       
            }
            return next_addr;
        }
        index = index + 1;
    }
    uart_puts("error\n");
    return -1 ;
}
void free_buddy_mem(unsigned long int t,int size)
{
    unsigned long int siz = my_pow(2,size);
    unsigned long int merge ;
    merge = t ^ ( siz << 12 );

    unsigned long int tmp = header[size];
    while(1)
    {
        print_mem(merge);
        uart_puts("\n");
        if(tmp == -1)//-1 insert
        {
            Node *p = t;
            p -> next = NULL;
            header[size] = p;
            return ;
        }
        else if(merge == header[size])
        {
            if(merge < t)
            {
                t = merge ;
            }
            Node *p = t;
            p = p -> next;
            if(p == NULL)
            {
                header[size] = -1; 
            }
            else
            {
                header[size] = p;
            }
            // print_mem(tmp);
            // uart_puts("     ");
            // print_int(size + 1);
            // uart_puts("\n");
            free_buddy_mem(t,size+1);
            return ;
        }
        else
        {
            Node *p = header[size];
            Node *q = p -> next;
            while(q != NULL)
            {
                if(q == merge)
                {
                    p -> next = q -> next;
                    if(merge < t)
                    {
                        t = merge ;
                    }
                    free_buddy_mem(t,size+1);
                    return ;
                }
                p = q ;
                q = q -> next;
            }
            Node * r = t ;
            p -> next = r;
            r -> next = NULL;
            return ; 
        }
        
    }
}












#define maxum_memory_pool_page_number 5

struct mem_pool_table
{
    //16,32,64,128
    unsigned long int bitmap[maxum_memory_pool_page_number];
    int page_frame_num;
    unsigned long int malloc_addr[maxum_memory_pool_page_number];
    //4096
    unsigned long int page_malloc_addr[5];
    int page_4096_num;
};
static struct mem_pool_table *mem_pool_table_pointer; 






void init_pool()
{
    mem_pool_table_pointer->bitmap[0] = 0x0;
    mem_pool_table_pointer->bitmap[1] = 0x0;
    mem_pool_table_pointer->bitmap[2] = 0x0;
    mem_pool_table_pointer->bitmap[3] = 0x0;
    mem_pool_table_pointer->bitmap[4] = 0x0;
    //mem_pool_pointer = require_buddy_mem(0);
    //mem_pool_table_pointer->malloc_addr[0] = mem_pool_pointer;
    mem_pool_table_pointer->page_frame_num = 0;
    //unsigned int a = require_buddy_mem(0);
    mem_pool_table_pointer->page_4096_num = 0;
}

void lookuppool()
{
    uart_puts("memory pool :\n");
    uart_puts("       addr\\siz                        16               32               64               128\n");
    for(int j=0;j<mem_pool_table_pointer->page_frame_num;j++)
    {
        
        unsigned long int tmp = mem_pool_table_pointer->bitmap[j];
        if(j<mem_pool_table_pointer->page_frame_num)
        {
            unsigned long int addr = mem_pool_table_pointer->malloc_addr[j];
            uart_puts("(");
            print_mem(addr);
            uart_puts(")");
            char c[30];
            int t=0;
            while(addr>0)
            {
                c[t] = '0' + addr %10 ;
                t = t + 1;
                addr = addr / 10;
            }
            while(t--)
            {
                uart_send(c[t]);
            }
            uart_puts("   ");

            for(int i=0;i<64;i++)
            {
                if(tmp%2 == 0)
                {
                    uart_send('O');
                }
                else
                {
                    uart_send('X');
                }
                if(i%16 == 15)
                {
                    uart_puts(" ");
                }
                tmp = tmp >> 1; 
            }
            uart_puts("\n");
        }
    }
    uart_puts("\n");
    uart_puts("       ");

    uart_puts("addr\\siz                     4096\n");
    for(int i=0;i<mem_pool_table_pointer->page_4096_num;i++)
    {
        unsigned long int addr = mem_pool_table_pointer->page_malloc_addr[i];
        uart_puts("(");
        print_mem(addr);
        uart_puts(")");
        char c[30];
        int t=0;
        while(addr>0)
        {
            c[t] = '0' + addr %10 ;
            t = t + 1;
            addr = addr / 10;
            }
        while(t--)
        {
            uart_send(c[t]);
        }
        uart_puts("        X\n");
    }
    uart_puts("\n");
}
    

unsigned long int my_malloc(int t)
{

    if(t > 128)
    {
        if(mem_pool_table_pointer->page_4096_num == maxum_memory_pool_page_number)
        {
            uart_puts("malloc error\n");
            return -1;
        }
        unsigned long int addr = require_buddy_mem(0);
        mem_pool_table_pointer->page_malloc_addr[mem_pool_table_pointer->page_4096_num] = addr;
        mem_pool_table_pointer->page_4096_num = mem_pool_table_pointer->page_4096_num + 1;
        return addr;
    }
    //16,32,64,128
    //mem_pool_table_pointer->bitmap ;
    int number_of_page = 0;
    if(mem_pool_table_pointer->page_frame_num == 0)
    {
        unsigned long int  mem_pool_pointer = require_buddy_mem(0);
        mem_pool_table_pointer->malloc_addr[0] = mem_pool_pointer;   
        mem_pool_table_pointer->page_frame_num = 1;
    }
    unsigned long int addr;
    while(1)
    {
        unsigned long int tmp = mem_pool_table_pointer->bitmap[number_of_page];
        unsigned long int filed;
        int index=0;
        if(t<=16)
        {
            filed = 0xFFFFFFFFFFFFFFFF;
            filed = filed ^ tmp;
            if(filed > 0)
            {
                while(((filed % 2) == 0))
                {
                    index = index + 1;
                    filed = filed / 2;
                }
                filed = 1;
                filed = filed << index;
                mem_pool_table_pointer->bitmap[number_of_page] = mem_pool_table_pointer->bitmap[number_of_page] | filed;
                //lookup_mem_pool();
                addr = mem_pool_table_pointer->malloc_addr[number_of_page] + 16 * index;   
                return  addr;
            }
        }
        else if(t<=32)
        {
            tmp = tmp >> 16;
            filed = 0xFFFFFFFFFFFF;
            filed = filed ^ tmp;
            if(filed > 0)
            {
                while(((filed % 2) == 0))
                {
                    index = index + 1;
                    filed = filed / 2;
                }
                filed = 1;
                filed = filed << (index + 16);
                mem_pool_table_pointer->bitmap[number_of_page] = mem_pool_table_pointer->bitmap[number_of_page] | filed;
                //lookup_mem_pool();   
                addr = mem_pool_table_pointer->malloc_addr[number_of_page] + 256 + 32 * index;   
                return  addr;
            }
        }
        else if(t<=64)
        {
            tmp = tmp >> 32;
            filed = 0xFFFFFFFF;
            filed = filed ^ tmp;
            if(filed > 0)
            {
                while(((filed % 2) == 0))
                {
                    index = index + 1;
                    filed = filed / 2;
                }
                filed = 1;
                filed = filed << (index + 32);
                mem_pool_table_pointer->bitmap[number_of_page] = mem_pool_table_pointer->bitmap[number_of_page] | filed;
                //lookup_mem_pool();   
                addr = mem_pool_table_pointer->malloc_addr[number_of_page] + 256 + 512 + 64 * index;   
                return  addr;
            }
        }
        else
        {
            tmp = tmp >> 48;
            filed = 0xFFFF;
            filed = filed ^ tmp;
            if(filed > 0)
            {
                while(((filed % 2) == 0))
                {
                    index = index + 1;
                    filed = filed / 2;
                }
                filed = 1;
                filed = filed << (index + 48);
                mem_pool_table_pointer->bitmap[number_of_page] = mem_pool_table_pointer->bitmap[number_of_page] | filed;
                //lookup_mem_pool();   
                addr = mem_pool_table_pointer->malloc_addr[number_of_page] + 256 + 512 + 1024 + 128 * index;   
                return  addr;
            }
        }

        //new frame page
        // struct mem_pool_table tmp = require_buddy_mem(0);

        //lookup_mem_pool();   
        number_of_page = number_of_page + 1;
        if( mem_pool_table_pointer->page_frame_num == number_of_page  && mem_pool_table_pointer->page_frame_num < maxum_memory_pool_page_number)
        {
            mem_pool_table_pointer->page_frame_num = mem_pool_table_pointer->page_frame_num + 1;
            mem_pool_table_pointer->bitmap[number_of_page] = 0x0;
            mem_pool_table_pointer->malloc_addr[number_of_page] = require_buddy_mem(0);
        }

        if(number_of_page == maxum_memory_pool_page_number)
        {
            uart_puts("not enough \n");
            return -1;

        }
    }
}


void my_free(unsigned long int t)
{
    //4096
    int total_4096_num = mem_pool_table_pointer->page_4096_num;
    int locate_4096_num = -1;
    for(int i =0 ;i<total_4096_num;i++)
    {
        if(mem_pool_table_pointer->page_malloc_addr[i] == t)
        {
            locate_4096_num = i;
            free_buddy_mem(t,0);
        }
    }
    if(locate_4096_num >= 0)
    {
        int page_4096_offset = mem_pool_table_pointer->page_4096_num - locate_4096_num - 1; 
        for(int i=0;i<page_4096_offset;i++)
        {
            mem_pool_table_pointer->page_malloc_addr[locate_4096_num + i] = mem_pool_table_pointer->page_malloc_addr[locate_4096_num + i + 1];
        }
        mem_pool_table_pointer->page_4096_num = mem_pool_table_pointer->page_4096_num - 1;
        return ;
    }




    //page1~5?
    int total_page_num = mem_pool_table_pointer->page_frame_num;
    int locate_page_num = -1;
    for(int i =0 ;i<total_page_num;i++)
    {
        if((mem_pool_table_pointer->malloc_addr[i] <= t) && (t < (mem_pool_table_pointer->malloc_addr[i] + 4096)))
        {
            locate_page_num = i;
        }
    }
    //not found
    if(locate_page_num == -1)
    {
        uart_puts("free error address\n");
        return ;
    }

    //release 16,32,64,128
    unsigned long int offset = t - mem_pool_table_pointer->malloc_addr[locate_page_num];
    unsigned long int map = 1;
    if(offset < 256)
    {
        map = (map << (offset / 16));
    } 
    else if((256 <= offset) && (offset < 768))//256+512=768
    {
        map = (map <<((offset - 256) / 32 + 16)); 
    }
    else if((768 <= offset) && (offset < 1792))//256+512+1024=1792
    {
        map = (map <<((offset - 768) / 64 + 32)); 
    }
    else
    {
        map = (map <<((offset - 1792) / 128 + 48)); 
    }

    mem_pool_table_pointer->bitmap[locate_page_num] = mem_pool_table_pointer->bitmap[locate_page_num] ^ map;
    //if bitmap == 0  buddy system
    if(locate_page_num >= 0)
    {
        if(mem_pool_table_pointer->bitmap[locate_page_num] == 0)
        {
            free_buddy_mem(mem_pool_table_pointer->malloc_addr[locate_page_num],0);
            int trans = mem_pool_table_pointer->page_frame_num - locate_page_num - 1;
            for(int i=0;i<trans;i++)
            {
                mem_pool_table_pointer->bitmap[locate_page_num + i] =  mem_pool_table_pointer->bitmap[locate_page_num + i + 1];
                mem_pool_table_pointer->malloc_addr[locate_page_num + i] = mem_pool_table_pointer->malloc_addr[locate_page_num + i + 1];
            }
            mem_pool_table_pointer->page_frame_num = mem_pool_table_pointer->page_frame_num - 1;
        }
    }
}





void default_mem (unsigned long int t, int size)
{





}