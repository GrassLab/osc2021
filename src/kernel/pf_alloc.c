#include "pf_alloc.h"
#include "string.h"
#include "def.h"
#include "io.h"
#include "math.h"

struct frame frame_arr[PF_ENTRY_LENGTH];
struct frame* head_arr[18]; // store the head of free list group by exp

void init_page_frame()
{
    printf("size: %d, %d\r\n", sizeof(long), sizeof(int));
    memset(head_arr, 0, sizeof(head_arr));

    frame_arr[0].idx = 0;
    frame_arr[0].exp = 17; // 2^17 = 131,072 entries
    frame_arr[0].next = NULL;

    for (int i = 1; i < PF_ENTRY_LENGTH; i++)
    {
        frame_arr[i].idx = i;
        frame_arr[i].exp = RESERVED_STATE;
        frame_arr[i].next = NULL;
    }

    head_arr[17] = &frame_arr[0];
}

// allocate (2 ^ exp) * 4KB
void *alloc_page(void *addr, short exp)
{
    short exp_tmp = exp;
    while (head_arr[exp_tmp] == NULL && exp_tmp <= 17)
    {
        exp_tmp++;
    }

    // No fit size
    if (exp_tmp > 17) {
        return NULL;
    }
    while (exp_tmp > exp) {
        // cut the size into half and appoint one part to lower size list
        struct frame *f_part1, *f_part2;
        f_part1 = head_arr[exp_tmp];
        f_part1->exp--;
        head_arr[exp_tmp] = f_part1->next;
        
        f_part2 = &frame_arr[f_part1->idx + int_pow(2, f_part1->exp)];
        f_part2->exp = f_part1->exp;
        f_part1->next = f_part2;
        
        append_to_list(f_part1, f_part1->exp);
        exp_tmp--;
    }
    
    //  sizeof int is not equal to sizeof void *, so hack here, using long before cast to void *
    addr = (void *)(long)(PHY_MEM_ALLOCABLE_START + head_arr[exp_tmp]->idx * PHY_PF_SIZE);
    
    // mark as allocated
    struct frame *next = head_arr[exp_tmp]->next;
    mark_as_allocated(head_arr[exp_tmp]);
    head_arr[exp_tmp] = next;

    mem_stat();

    return addr;
}

void mark_as_allocated(struct frame *f)
{   
    for (int i = 0; i < int_pow(2, f->exp); i++) {
        frame_arr[f->idx + i].exp = ALLOCATED_STATE;
        frame_arr[f->idx + i].next = NULL;
    }
}

void mem_stat()
{
    printf("----- memory status -----\r\n");
    for (int i = 0; i <= 17; i++) {
        int count = 0;
        struct frame *f = head_arr[i];
        while(f != NULL) {
            count++;
            f = f->next;
        }

        printf("2^%d : %d left\r\n", i, count);
    }
    printf("-------------------------\r\n\n");
}

void free_page(void *start, short exp)
{
    // calculate the index
    long idx = ((long)start - PHY_MEM_ALLOCABLE_START) / PHY_PF_SIZE;
    printf("%l\r\n", idx);
    
    // first block set size
    frame_arr[idx].exp = exp;

    // other blocks set reserved
    for (int i = 1; i < int_pow(2, exp); i++) {
        frame_arr[idx + i].exp = RESERVED_STATE;
    }

    append_to_list(&frame_arr[idx], exp);

    try_merge(exp);

    mem_stat();
}

// merge start from 2 ^ exp
void try_merge(short exp)
{
    printf("merging\n");
    if (exp >= 17) {
        return;
    }

    if (!head_arr[exp]) {
        return;
    }
    
    struct frame *prev, *cur = head_arr[exp];
    int distance = int_pow(2, exp);

    while(cur && cur->idx % (2 * distance) == 0) {
        printf("exist!\n");
        struct frame *tmp = cur->next, *next = cur->next;


        while (tmp) {
            // check next index is in list
            if (tmp->idx == (cur->idx + distance)) {
                // can merge, clear state
                cur->exp *= 2;
                cur->next = NULL;

                tmp->exp = RESERVED_STATE;
                tmp->next = NULL;

                prev->next = tmp->next;

                // add to higher size list
                append_to_list(cur, cur->exp);
            }

            tmp = tmp->next;
        }

        prev = cur;
        cur = next;
    }

    try_merge(exp + 1);
}

void append_to_list(struct frame *f, short exp)
{
    if (head_arr[exp]) {
        struct frame *cur = head_arr[exp];
        while(cur->next) {
            cur = cur->next;
        }
        cur->next = f;
    } else {
        head_arr[exp] = f;
    }
}