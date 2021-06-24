#include "mmu.h"
#include "uart.h"
#include "thread.h"

// 初始化頁表
void init_page_table(struct thread *thread, unsigned long **table) 
{
	// 配置4KB
	*table = (unsigned long *)thread_allocate_page(thread, 4096);
	
	for (int i = 0; i < 512; i++) 
		*((*table) + i) = 0;

	// 虛擬位址轉實體位址
	*table = (unsigned long *)VA2PA(*table);
}

// 為process更新頁表
void update_page_table(struct thread *thread, unsigned long virtual_addr, unsigned long physical_addr, int permission) 
{
	// PGD為空錯誤
	if (thread->pgd == 0) 
	{
		uart_putstr("Invalid PGD!!\n");
		return;
	}
	
	// 取得各層，PGD: 2^39, PUD: 2^30, PMD: 2^21, PTE = 2^12 = 4KB
	unsigned int index[4] = 
	{
		(virtual_addr >> 39) & 0x1ff, (virtual_addr >> 30) & 0x1ff,
		(virtual_addr >> 21) & 0x1ff, (virtual_addr >> 12) & 0x1ff
	};

	unsigned long *table = (unsigned long *)PA2VA(thread->pgd);

	// 由階層0-2, PGD=>PUD=>PMD
	for (int level = 0; level < 3; level++) 
	{
		// 假如下一級頁表尚未出現
		if (table[index[level]] == 0) 
		{
			// 配置一個頁框以作下一級頁表
			init_page_table(thread, (unsigned long **)&(table[index[level]]));
			table[index[level]] |= PD_TABLE;
		}

		// 轉為virtual address
		table = (unsigned long *)PA2VA(table[index[level]] & ~0xfff);
	}
	
	// 設置讀取寫入權限
	unsigned long BOOT_RWX_ATTR = (1 << 6);
	if (permission & 0b010)
		BOOT_RWX_ATTR |= 0;
	else
		BOOT_RWX_ATTR |= (1 << 7);

	table[index[3]] = physical_addr | BOOT_PTE_NORMAL_NOCACHE_ATTR | BOOT_RWX_ATTR;
}