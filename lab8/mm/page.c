#include "page.h"
#include <types.h>
#include <varied.h>
#include <printf.h>

void* page_alloc(void* addr, size_t size) {
  size_t* pgd_addr, *pud_addr, *pmd_addr, *pte_addr;
  size_t total_pgd_entry, total_pud_entry, total_pmd_entry, total_pte_entry;  
  size_t num_of_pud_entry, num_of_pmd_entry, num_of_pte_entry;
  float temp;
  

  temp = size / PAGE_SIZE;
  total_pte_entry = (int)temp;
  if(temp != (float)total_pte_entry)
    total_pte_entry += 1;

  temp = total_pte_entry / NUM_OF_ENTRY_PER_TABLE_4KB;
  total_pmd_entry = (int)temp;
  if(temp != (float)total_pmd_entry)
    total_pmd_entry += 1;

  if(total_pmd_entry == 0)
    total_pmd_entry += 1;
  
  temp = total_pmd_entry / NUM_OF_ENTRY_PER_TABLE_4KB;
  total_pud_entry = (int)temp;
  if(temp != (float)total_pud_entry)
    total_pud_entry += 1;

  if(total_pud_entry == 0)
    total_pud_entry += 1;
  
  temp = total_pud_entry / NUM_OF_ENTRY_PER_TABLE_4KB;
  total_pgd_entry = (int)temp;
  if(temp != (float)total_pgd_entry)
    total_pgd_entry += 1;

  if(total_pgd_entry == 0)
    total_pgd_entry += 1;
  printf("pgd: %d, pud: %d, pmd: %d, pte: %d\n", total_pgd_entry, total_pud_entry, total_pmd_entry, total_pte_entry);
  //pgd table
  pgd_addr = varied_malloc(PAGE_SIZE);
  
  if(pgd_addr == null)
    return null;
 
  printf("pgd_addr: %x\n", pgd_addr);
 
  //tarverse pgd
  for(int i = 0; i < total_pgd_entry; i++) {
    
    pud_addr = varied_malloc(PAGE_SIZE);

    if(pud_addr == null)
      return null;
    
    printf("pud_addr: %x\n", pud_addr);
  
    //set pgd entry
    pgd_addr[i] = pd_encode_table(pud_addr);
    
    if(total_pud_entry >= NUM_OF_ENTRY_PER_TABLE_4KB) {
      num_of_pud_entry = NUM_OF_ENTRY_PER_TABLE_4KB;
      total_pud_entry -= NUM_OF_ENTRY_PER_TABLE_4KB;
    }
    else 
      num_of_pud_entry = total_pud_entry;
    //traverse pud table pgd_addr[i]
    for(int j = 0; j < num_of_pud_entry; j++) {
      
      pmd_addr = varied_malloc(PAGE_SIZE);

      if(pmd_addr == null)
        return null;
      
      printf("pmd_addr: %x\n", pmd_addr);
 
      //set pud entry
      pud_addr[j] = pd_encode_table(pmd_addr);
      
      if(total_pmd_entry >= NUM_OF_ENTRY_PER_TABLE_4KB) {
        num_of_pmd_entry = NUM_OF_ENTRY_PER_TABLE_4KB;
        total_pmd_entry -= NUM_OF_ENTRY_PER_TABLE_4KB;
      }
      else 
        num_of_pmd_entry = total_pmd_entry;
      //traverse pmd table 
      //should start from 0x2
      for(int k = 0; k < num_of_pmd_entry; k++) {

        pte_addr = varied_malloc(PAGE_SIZE);

        if(pte_addr == null)
          return null;
    
        printf("pte_addr: %x\n", pte_addr); 
        //set pmd entry
        if(j == 0) {
          //start from 0x400000
          pmd_addr[k+2] = pd_encode_table(pte_addr);
          pmd_addr[0] = pd_encode_ram_block(0x0000);
        }
        else
          pmd_addr[k] = pd_encode_table(pte_addr);

        if(total_pte_entry >= NUM_OF_ENTRY_PER_TABLE_4KB) {
          num_of_pte_entry = NUM_OF_ENTRY_PER_TABLE_4KB;
          total_pte_entry -= NUM_OF_ENTRY_PER_TABLE_4KB;
        }
        else 
          num_of_pte_entry = total_pte_entry;
        //traverse pte table
        for(int l = 0; l < num_of_pte_entry; l++) {
          //set pte entry
          pte_addr[l] = pd_encode_ram(get_physical_addr(addr, i, j, k, l));
          printf("i: %d, j: %d, k: %d, l: %d, physical address: %x\n", i, j, k, l, get_physical_addr(addr, i, j, k, l));
        }
      }
    }
  }
  

  //map stack to 0x00007ffffffff000
  //last pud
  if(pgd_addr[NUM_OF_ENTRY_PER_TABLE_4KB-1] == null) {
  
    pud_addr = varied_malloc(PAGE_SIZE);
  
    if(pud_addr == null)
      return pgd_addr;
  }
  else {
    pud_addr = pgd_addr[NUM_OF_ENTRY_PER_TABLE_4KB-1];
  }
  
  if(pud_addr[NUM_OF_ENTRY_PER_TABLE_4KB-1] == null) {

    pmd_addr = varied_malloc(PAGE_SIZE);

    if(pmd_addr == null)
      return pgd_addr;
  }
  else {
    pmd_addr = pud_addr[NUM_OF_ENTRY_PER_TABLE_4KB-1];
  }
  
  if(pmd_addr[NUM_OF_ENTRY_PER_TABLE_4KB-1] == null) {

    pte_addr = varied_malloc(PAGE_SIZE);

    if(pte_addr == null)
      return pgd_addr;
  }
  else {
    pte_addr = pmd_addr[NUM_OF_ENTRY_PER_TABLE_4KB-1];
  }
        
  return pgd_addr;
}

void* get_physical_addr(void* addr, int i, int j, int k, int l) {
  return addr + PAGE_SIZE * ( l + NUM_OF_ENTRY_PER_TABLE_4KB * k + NUM_OF_ENTRY_PER_TABLE_4KB * NUM_OF_ENTRY_PER_TABLE_4KB * j +  NUM_OF_ENTRY_PER_TABLE_4KB * NUM_OF_ENTRY_PER_TABLE_4KB * NUM_OF_ENTRY_PER_TABLE_4KB * i); 
}




int map_phys_to_virt(void* addr, size_t size) {
 
  //allocate page frame for four level translation
  page_alloc(addr, size);
  //allocate space for user process memory

  //map physical address to virtual address

  return null;
}