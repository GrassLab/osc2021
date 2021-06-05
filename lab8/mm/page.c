#include "page.h"
#include <types.h>
#include <varied.h>
#include <printf.h>
#include <sched.h>

void* page_map_binary(void* addr, size_t size, size_t* ctx_pgd) {
  size_t* pgd_addr, *pud_addr, *pmd_addr, *pte_addr;
  size_t total_pgd_entry, total_pud_entry, total_pmd_entry, total_pte_entry;  
  size_t num_of_pud_entry, num_of_pmd_entry, num_of_pte_entry;

  total_pte_entry = page_cal_total_page_entry(size, PAGE_SIZE); 
  total_pmd_entry = page_cal_total_page_entry(total_pte_entry, NUM_OF_ENTRY_PER_TABLE_4KB);
  total_pud_entry = page_cal_total_page_entry(total_pmd_entry, NUM_OF_ENTRY_PER_TABLE_4KB);
  total_pgd_entry = page_cal_total_page_entry(total_pud_entry, NUM_OF_ENTRY_PER_TABLE_4KB);
  
  //printf("pgd: %d, pud: %d, pmd: %d, pte: %d\n", total_pgd_entry, total_pud_entry, total_pmd_entry, total_pte_entry);
  
  //pgd table
  pgd_addr = page_pgd_allocate(ctx_pgd);
  
  //printf("pgd_addr: %x\n", pgd_addr);
 
  //tarverse pgd
  for(int i = 0; i < total_pgd_entry; i++) {
    
    pud_addr = page_allocate(pgd_addr, i);

    if(pud_addr == null)
      return pgd_addr;
    
    //printf("pud_addr: %x\n", pud_addr);

    num_of_pud_entry = page_get_num_of_table_entry(&total_pud_entry);
    //traverse pud table pgd_addr[i]
    for(int j = 0; j < num_of_pud_entry; j++) {
      
      pmd_addr = page_allocate(pud_addr, j);
      
      if(pmd_addr == null)
        return pgd_addr;

      //printf("pmd_addr: %x\n", pmd_addr);
      
      num_of_pmd_entry = page_get_num_of_table_entry(&total_pmd_entry);
     
      //traverse pmd table 
      //should start from 0x2
      for(int k = 0; k < num_of_pmd_entry; k++) {

        pte_addr = page_allocate(pmd_addr, k);
        
        if(pte_addr == null)
          return pgd_addr;

       //printf("pte_addr: %x\n", pte_addr); 
        //set pmd entry
        if(j == 0) {
          //start from 0x400000
          pmd_addr[k+2] = pd_encode_table(pte_addr);
          pmd_addr[0] = pd_encode_ram_block(0x0000);
        }
        
        num_of_pte_entry = page_get_num_of_table_entry(&total_pte_entry);
        //traverse pte table
        for(int l = 0; l < num_of_pte_entry; l++) {
          //set pte entry
          pte_addr[l] = pd_encode_ram(get_physical_addr(addr, i, j, k, l));
          //printf("i: %d, j: %d, k: %d, l: %d, physical address: %x\n", i, j, k, l, get_physical_addr(addr, i, j, k, l));
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
  //printf("pgd_addr: %x, pud_addr: %x, pmd_addr: %x, pte_addr: %x\n", pgd_addr, pud_addr, pmd_addr, pte_addr);
}

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

void* get_physical_addr(void* addr, int i, int j, int k, int l) {
  return addr + PAGE_SIZE * ( l + NUM_OF_ENTRY_PER_TABLE_4KB * k + NUM_OF_ENTRY_PER_TABLE_4KB * NUM_OF_ENTRY_PER_TABLE_4KB * j +  NUM_OF_ENTRY_PER_TABLE_4KB * NUM_OF_ENTRY_PER_TABLE_4KB * NUM_OF_ENTRY_PER_TABLE_4KB * i); 
}
