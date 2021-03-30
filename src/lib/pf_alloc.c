#include "pf_alloc.h"
#include "string.h"
#include "def.h"
#include "mini_uart.h"

struct frame frame_arr[PF_ENTRY_LENGTH];
struct frame* head_arr[17]; // store the head of free list group by exp

void init_page_frame()
{
    memset(head_arr, 0, sizeof(head_arr));

    frame_arr[0].idx = 0;
    frame_arr[0].exp = 17; // 2^17 = 131,072 entries

    for (int i = 1; i < PF_ENTRY_LENGTH; i++)
    {
        frame_arr[i].idx = i;
        frame_arr[i].exp = RESERVED_STATE;
        frame_arr[i].next = NULL;
    }

    head_arr[16] = &frame_arr[0];
}

// allocate (2 ^ exp) * 4KB
void *alloc_page(short exp)
{
    short exp_tmp = exp;
    while (head_arr[exp_tmp] == NULL && exp_tmp < 17)
    {
        exp_tmp++;
    }

    // No fit size
    if (exp_tmp >= 17) {
        return NULL;
    }

    puts("allocate ");
    char value_str[100];
    puts(itoa(exp, value_str, 10));
    puts("\n");

    while (exp_tmp > exp) {
        struct frame *f_part1, *f_part2;
        
        f_part1 = head_arr[exp_tmp];
        head_arr[exp_tmp] = f_part1->next;
        
        // cut and build the second part
        f_part2 = &frame_arr[f_part1->idx + (2 ^ (f_part1->exp - 1))];
        f_part2->exp = --f_part1->exp;
        f_part1->next = f_part2;
        
        head_arr[exp_tmp - 1] = f_part1;

        exp_tmp--;
    }
    
    // sizeof int is not equal to sizeof void *, so hack here, using long before cast to void *
    void *pRtn = (void *)(long)(PHY_MEM_ALLOCABLE_START + head_arr[exp_tmp]->idx * PHY_PF_SIZE);
    // mark as allocated
    mark_allocated(head_arr[exp_tmp]);
    head_arr[exp_tmp] = head_arr[exp_tmp]->next;

    return pRtn;
}

// TODO: Free
void mark_allocated(struct frame *f)
{   
    for (int i = 0; i < (2 ^ f->exp); i++) {
        frame_arr[f->idx + i].exp = ALLOCATED_STATE;
    }

}