#include "page.h"
#include <types.h>
#include <varied.h>
#include <printf.h>
#include <sched.h>

void* page_map_binary(void* addr, size_t size, size_t* ctx_pgd) {
  size_t* pgd_addr, *pud_addr, *pmd_addr, *pte_addr;
  size_t total_pgd_entry, total_pud_entry, total_pmd_entry, total_pte_entry;  
  size_t num_of_pud_entry, num_of_pmd_entry, num_of_pte_entry;
  
  //calculate total number of each level entry
  total_pte_entry = page_cal_total_page_entry(size, PAGE_SIZE); 
  total_pmd_entry = page_cal_total_page_entry(total_pte_entry, NUM_OF_ENTRY_PER_TABLE_4KB);
  total_pud_entry = page_cal_total_page_entry(total_pmd_entry, NUM_OF_ENTRY_PER_TABLE_4KB);
  total_pgd_entry = page_cal_total_page_entry(total_pud_entry, NUM_OF_ENTRY_PER_TABLE_4KB);
   
  //allocate pgd table
  pgd_addr = page_pgd_allocate(ctx_pgd);
  
  //tarverse pgd
  for(int i = 0; i < total_pgd_entry; i++) {
    
    //allocate pud table
    pud_addr = page_allocate(pgd_addr, i);

    if(pud_addr == null)
      return pgd_addr;
    
    //get number of entry in this table
    num_of_pud_entry = page_get_num_of_table_entry(&total_pud_entry);
    //traverse pud table 
    for(int j = 0; j < num_of_pud_entry; j++) {
      
      //allocate pmd table
      pmd_addr = page_allocate(pud_addr, j);
      
      if(pmd_addr == null)
        return pgd_addr;

      //get number of entry in this table
      num_of_pmd_entry = page_get_num_of_table_entry(&total_pmd_entry);
     
      //traverse pmd table 
      for(int k = 0; k < num_of_pmd_entry; k++) {

        pte_addr = page_allocate(pmd_addr, k);
        
        if(pte_addr == null)
          return pgd_addr;

        //if is in first pud table and first pmd table, should start from 0x2
        if(j == 0 && i == 0) {
          //start from 0x400000
          pmd_addr[k+2] = pd_encode_table(pte_addr);
          //map exception table, cannot be execute
          pmd_addr[0] = pd_encode_ram_block(0x0000);
        }
        
        num_of_pte_entry = page_get_num_of_table_entry(&total_pte_entry);
        //traverse pte table
        for(int l = 0; l < num_of_pte_entry; l++) {
          //set pte entry
          pte_addr[l] = pd_encode_ram(get_physical_addr(addr, i, j, k, l));
        }
      }
    }
  }
  
  return pgd_addr;
}

void page_map_stack(void* stack_addr, size_t* ctx_pgd) {
  size_t* pgd_addr, *pud_addr, *pmd_addr, *pte_addr;
  
  //map stack to 0x00007ffffffff000
  //last pud
  pgd_addr = page_pgd_allocate(ctx_pgd);
  pud_addr = page_allocate(pgd_addr, 255);
  pmd_addr = page_allocate(pud_addr, NUM_OF_ENTRY_PER_TABLE_4KB-1);
  pte_addr = page_allocate(pmd_addr, NUM_OF_ENTRY_PER_TABLE_4KB-1);

  pte_addr[NUM_OF_ENTRY_PER_TABLE_4KB-1] = pd_encode_ram(stack_addr);
  pte_addr[NUM_OF_ENTRY_PER_TABLE_4KB-2] = pd_encode_ram(stack_addr - PAGE_SIZE);
}

//calculate total number of entry in each level table
size_t page_cal_total_page_entry(size_t total_low_level_entry, size_t table_size) {
  size_t total_high_level_entry;
  float temp;

  temp = total_low_level_entry / table_size;
  total_high_level_entry = (int)temp;
  if(temp != (float)total_high_level_entry)
    total_high_level_entry += 1;
  
  if(total_high_level_entry == 0)
    total_high_level_entry += 1;
  
  return total_high_level_entry;
}
//get number of entrys in this table
size_t page_get_num_of_table_entry(size_t *total_low_level_entry) {
  size_t num_of_entry;
  if(*total_low_level_entry >= NUM_OF_ENTRY_PER_TABLE_4KB) {
      num_of_entry = NUM_OF_ENTRY_PER_TABLE_4KB;
      *total_low_level_entry -= NUM_OF_ENTRY_PER_TABLE_4KB;
    }
    else 
      num_of_entry = *total_low_level_entry;
  return num_of_entry;
}
//allocate pgd table, if ctx_gpd is null, otherwise reuse it
void* page_pgd_allocate(size_t* ctx_pgd) {
  void* pgd_addr;
  if(*ctx_pgd == null) {
    
    pgd_addr = varied_malloc(PAGE_SIZE);
  
    if(pgd_addr == null)
      return pgd_addr;

    *ctx_pgd = pd_encode_addr((size_t)pgd_addr);
  }
  else {
    pgd_addr = (void* )pd_decode_addr(*ctx_pgd);
  }

  return pgd_addr;
}
//allocate pud, pmd, pte table, if null, otherwise reuse it
void* page_allocate(size_t* par_addr, int idx) {
  void* new_addr;
  if(par_addr[idx] == null) {

    new_addr = varied_malloc(PAGE_SIZE);

    if(new_addr == null)
      return new_addr;

    par_addr[idx] = pd_encode_table(new_addr);
  }
  else {
    new_addr = (void* )pd_decode_addr(par_addr[idx]);
  }

  return new_addr;
}
//calculate physicall address from virtual and offset
void* get_physical_addr(void* addr, int i, int j, int k, int l) {
  return addr + PAGE_SIZE * ( l + NUM_OF_ENTRY_PER_TABLE_4KB * k + NUM_OF_ENTRY_PER_TABLE_4KB * NUM_OF_ENTRY_PER_TABLE_4KB * j +  NUM_OF_ENTRY_PER_TABLE_4KB * NUM_OF_ENTRY_PER_TABLE_4KB * NUM_OF_ENTRY_PER_TABLE_4KB * i); 
}

//when user process exit, free page table
void page_free(size_t* ctx_pgd, size_t size) {
  /**
   * should free page table
   */
}