# include "page.h"
# include "uart.h"
# include "my_math.h"

extern unsigned char __kernel_page_start;

void kernel_page_setup(){
  uint64_t kernel_page_start = (uint64_t) (&__kernel_page_start) & (0xffffffffffff);
  uint64_t pge_start_addr = kernel_page_start;
  uint64_t pue_start_addr = kernel_page_start + PAGE_SIZE;
  uint64_t pme_start_addr = kernel_page_start + PAGE_SIZE*2;
  uint64_t pte_start_addr = kernel_page_start + PAGE_SIZE*3;
  struct pg_t *pge = (struct pg_t*) pge_start_addr;
  struct pg_t *pue = (struct pg_t*) pue_start_addr;
  struct pg_t *pme = (struct pg_t*) pme_start_addr;
  struct pg_t *pte_l[PD_LEN];
  for (int i=0; i<PD_LEN; i++){
    pte_l[i] = (struct pg_t*) (pte_start_addr + PAGE_SIZE*i);
  }
  
  pge->pg_data[0] = (pue_start_addr | PGD_ATTR);
  pue->pg_data[0] = (pme_start_addr | PUD0_ATTR);
  pue->pg_data[1] = ((0x40000000) | PUD1_ATTR);
  for (int i=0; i<PD_LEN; i++){
    pme->pg_data[i] = ( (pte_start_addr+PAGE_SIZE*i) | PME_ATTR);
  }
  
  for (int n=0; n<PD_LEN-8; n++){
    struct pg_t *pte = pte_l[n];
    for (int i=0; i<PD_LEN; i++){
      pte->pg_data[i] = ( ((n*PD_LEN+i)*PAGE_SIZE) | PTE_NORMAL_ATTR);
    }
  }
  for (int n=PD_LEN-8; n<PD_LEN; n++){
    struct pg_t *pte = pte_l[n];
    for (int i=0; i<PD_LEN; i++){
      pte->pg_data[i] = ( ((n*PD_LEN+i)*PAGE_SIZE) | PTE_DEVICE_ATTR);
    }
  }
}
