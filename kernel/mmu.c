#include "allocator.h"
#include "scheduler.h"
#include "mmu.h"
#include "base.h"

void map_table_entry(unsigned long *pte, unsigned long va, unsigned long pa) {
	unsigned long index = va >> PAGE_SHIFT;
	index = index & (PTRS_PER_TABLE - 1);
	//unsigned long entry = pa | MMU_PTE_FLAGS; 
	unsigned long entry = (pa & 0x0000ffffffffffff) | MMU_PTE_FLAGS;
    pte[index] = entry;
}

unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, int* new_table) {
	unsigned long index = va >> shift;
	index = index & (PTRS_PER_TABLE - 1);
	if (!table[index]){
		*new_table = 1;
		unsigned long next_level_table = kmalloc(1<<12);
        memzero(next_level_table, PAGE_SIZE);
		//unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE;
		unsigned long entry = (next_level_table & 0x0000ffffffffffff) | MM_TYPE_PAGE_TABLE;
        table[index] = entry;
		return next_level_table;
	} else {
		*new_table = 0;
	}
	return (table[index] & PAGE_MASK) | 0xffff000000000000;
}

void map_page(struct task_struct *task, unsigned long va, unsigned long page){
	unsigned long pgd;
	if (!task->mm.pgd) {
		pgd = kmalloc(1<<12);
        memzero(pgd, PAGE_SIZE);
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pgd;
        task->mm.pgd = pgd & 0x0000ffffffffffff; // task.mm->pgd should be 0x0000.....
	}
	pgd = task->mm.pgd | 0xffff000000000000; // pgd should be 0xffff....
    
	int new_table;
	unsigned long pud = map_table((unsigned long *)(pgd), PGD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pud;
	}
	unsigned long pmd = map_table((unsigned long *)(pud) , PUD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pmd;
	}
	unsigned long pte = map_table((unsigned long *)(pmd), PMD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pte;
	}
	map_table_entry((unsigned long *)(pte), va, page);
	struct user_page p = {page, va};
	task->mm.user_pages[task->mm.user_pages_count++] = p;
}

unsigned long allocate_user_page(struct task_struct *task, unsigned long va) {
	unsigned long page = kmalloc(1<<12);
	if (page == 0) {
		return 0;
	}
	map_page(task, va, page);
    
	return page;
}

int copy_virt_memory(struct task_struct *dst) {
	struct task_struct* src = current;
	for (int i = 0; i < src->mm.user_pages_count; i++) {
		unsigned long kernel_va = allocate_user_page(dst, src->mm.user_pages[i].virt_addr);
		if( kernel_va == 0) {
			return -1;
		}
		memcpy(kernel_va, src->mm.user_pages[i].virt_addr, PAGE_SIZE);
	}
	return 0;
}
