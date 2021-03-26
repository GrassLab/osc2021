#include <stdlib.h>
#include <uart.h>
#include <utils.h>
#include <string.h>
typedef unsigned long ulong;

extern ulong __heap_start; /* declared in the linker script */
extern ulong __heap_size;  /* declared in the linker script */
static ulong heap_top = (ulong) &__heap_start;
static ulong heap_size = (ulong) &__heap_size;
static ulong *curr_top = (ulong *) 0xFFFFFFF0, *heap_end = (ulong *) 0xFFFFFFF0;
        
void *malloc(size_t n)
{
    void *return_ptr;
    ulong *ptr, temp;
    int r;

    if (curr_top == heap_end) // first time to call malloc()?
    {
        curr_top = (ulong *) heap_top;
        heap_end = (ulong *) ((heap_top + heap_size) & 0xFFFFFFF0);
        *curr_top = (ulong) heap_end;
    }

    // Search for a large-enough free memory block (FMB).
    return_ptr = NULL;
    for (ptr = curr_top; ptr < heap_end; ptr = (ulong *) *ptr)
    {   
        if ((*ptr & 1) == 0 && (*ptr - (ulong) ptr > n))
        {
            temp = ((ulong) ptr) & 0xFFFFFFFE;
            return_ptr = (void *) (temp + sizeof(ulong));

            // Update the FMB link list structure.
            r = n % sizeof(ulong);
            temp = n + sizeof(ulong) + ((r)? 4-r : 0);
            curr_top = ptr + temp/sizeof(ulong);
            *curr_top = *ptr;
            *ptr = (ulong) curr_top | 1;
            break;
        }
    }

    if (return_ptr != NULL) return return_ptr;

    // Search again for a FMB from heap_top to curr_top
    for (ptr = (ulong *) heap_top; ptr < curr_top; ptr = (ulong *) *ptr)
    {
        if ((*ptr & 1) == 0 && (*ptr - (ulong) ptr > n))
        {
            temp = ((ulong) ptr) & 0xFFFFFFFE;
            return_ptr = (void *) (temp + sizeof(ulong));

            // Update the FMB link list structure.
            r = n % sizeof(ulong);
            temp = n + sizeof(ulong) + ((r)? 4-r : 0);
            curr_top = ptr + temp/sizeof(ulong);
            *curr_top = *ptr;
            *ptr = (ulong) curr_top | 1;
            break;
        }
    }

    return return_ptr;
}

void free(void *mptr)
{
    ulong *ptr, *next;

    ptr = ((ulong *) mptr) - 1;
    *ptr = *ptr & 0xFFFFFFFE; // Free the FMB.
    next = (ulong *) *ptr;
    if ((*next & 1) == 0)
    {
        *ptr = *next; // Merge with the next FMB.
        curr_top = ptr;
    }
}

void *calloc(size_t n, size_t size)
{
    void *mptr;
    mptr = malloc(n*size);
    memset(mptr, 0, n*size);
    return mptr;
}

int abs(int n)
{
    int j;

    if (n >= 0) j = n; else j = -n;

	return j;
}

