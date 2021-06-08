# include "page.h"
# include "uart.h"
# include "my_math.h"
# include "mem.h"
# include "mem_addr.h"
# include "log.h"
# include "buddy.h"

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

uint64_t get_kernel_ttbr0(){
  uint64_t kernel_page_start = (uint64_t) (&__kernel_page_start) & (0xffffffffffff);
  return kernel_page_start;
}

static inline int64_t page_action_pte(uint64_t v_addr, struct pg_t *pte, list_head *head, int flag){
  uint16_t pa_offset = v_addr & (PAGE_SIZE-1);
  uint16_t pte_offset = (v_addr >> PAGE_SIZE_BITS) & (PD_LEN-1);
  uint64_t data = pte->pg_data[pte_offset];
  switch(flag){
    case VtoP:
      if (data & 0x1){
        uint64_t pa_base = data & (~( (PAGE_SIZE-1) | KVA ));
        return (int64_t)(pa_base+pa_offset);
      }
      return -1;
    case RM:
      if (data & 0x1){
        uint64_t pa_base = ((data & (~(PAGE_SIZE-1)) ) | KVA);
        free((void*)pa_base);
        pte->pg_data[pte_offset] = 0;
        return 0;
      }
      return -1;
    case VtoP_CREAT:
      if (data & 0x1){
        uint64_t pa_base = data & (~( (PAGE_SIZE-1) | KVA ));
        return (int64_t)(pa_base+pa_offset);
      }
      else{
        uint64_t pa_base = (uint64_t) malloc(PAGE_SIZE, 1);
        if (pa_base == 0) return -1;
        pa_base &= (~( (PAGE_SIZE-1) | KVA ));
        pte->pg_data[pte_offset] = (pa_base | PTE_USER_ATTR);
        return (int64_t)(pa_base+pa_offset);
      }
    case RM_ALL:
    case GET_ALL:
      for (uint64_t i=0; i<PD_LEN; i++){
        data = pte->pg_data[i];
        if (data & 0x1){
          uint64_t base = ((data & (~(PAGE_SIZE-1)) ) | KVA);
          if (flag == RM_ALL){
            pte->pg_data[i] = 0;
            free((void*) base);
          }
          else{
            if (!head) return -1;
            struct pg_list *new_node = MALLOC(struct pg_list, 1);
            new_node->pa = base - KVA;
            new_node->va = (v_addr | (i << PAGE_SIZE_BITS));
            list_add_prev(&(new_node->head), head);
          }
        }
      }
      if (flag == RM_ALL) free(pte);
      return 0;
    default:
      return -1;
  }
}

static int64_t page_action_rec(uint64_t v_addr, struct pg_t *pxe, list_head *head, enum page_act flag, int levels = 3){
  if (levels == 0) return page_action_pte(v_addr, pxe, head, flag);
  
  switch (flag){
    case VtoP:
    case VtoP_CREAT:
    case RM:
    {
      uint16_t offset = (v_addr >> (PAGE_SIZE_BITS+PD_LEN_BITS*levels)) & (PD_LEN-1);
      uint64_t data = pxe->pg_data[offset];
      if (data & 0x1){
        uint64_t base = (data & (~(PAGE_SIZE-1)) ) | KVA;
        return page_action_rec(v_addr, (struct pg_t*)base , head, flag, levels-1);
      }
      else if (flag == VtoP_CREAT){
        uint64_t base = (uint64_t) malloc(PAGE_SIZE, 1);
        if (base == 0) return -1;
        uint64_t pa_base = base & (~(KVA));
        pxe->pg_data[offset] = (pa_base | PXE_USER_ATTR);
        return page_action_rec(v_addr, (struct pg_t*)base , head, flag, levels-1);
      }
      return -1;
    } 
    case GET_ALL:
    case RM_ALL:
      for (uint64_t i=0; i<PD_LEN; i++){
        uint64_t data = pxe->pg_data[i];
        if (data & 0x1){
          uint64_t base = (data & (~(PAGE_SIZE-1)) ) | KVA;
          uint64_t v_addr_t = (v_addr | (i << (PAGE_SIZE_BITS+PD_LEN_BITS*levels)));
          int64_t rt = page_action_rec(v_addr_t, (struct pg_t*)base, head, flag, levels-1);
          if (rt < 0) return rt;
          if (flag == RM_ALL){
            pxe->pg_data[i] = 0;
          }
        }
      }
      if (flag == RM_ALL && levels < 3) free(pxe);
      return 0;
    default:
      return -1;
  }
}

int64_t create_user_page(uint64_t v_addr, uint64_t ttbr0){
  return page_action_rec(v_addr, (struct pg_t*)ttbr0, 0, VtoP_CREAT);
}

int64_t rmall_user_page(uint64_t ttbr0){
  return page_action_rec(0x0, (struct pg_t*) ttbr0, 0, RM_ALL);
}

int64_t getall_user_page(void* ttbr0, list_head *head){
  return page_action_rec(0, (struct pg_t*)ttbr0, head, GET_ALL);
}

int64_t va_to_pa(uint64_t va, void* ttbr0){
  return page_action_rec(va, (struct pg_t*)ttbr0, 0, VtoP);
}

void user_pt_show(void* ttbr){
  list_head head;
  list_head_init(&head);
  page_action_rec(0, (struct pg_t*)ttbr, &head, GET_ALL);
  uart_puts("         Virtual|      Physical |\n");
  uart_puts("================|===============|\n");
  char addr_ct[35];
  for (int i=0; i<35; i++) addr_ct[i] = ' ';
  list_head *pos;
  if (list_is_empty(&head)){
    log_puts("Empty\n", INFO);
  }
  list_for_each(pos, &head){
    struct pg_list *t = container_of(pos, struct pg_list, head);
    int_to_hex_align(t->va, addr_ct, 15);
    int_to_hex_align(t->pa, addr_ct+16, 15);
    addr_ct[16] = '|';
    addr_ct[32] = '|';
    addr_ct[33] = '\n';
    addr_ct[34] = '\0';
    uart_puts(addr_ct);
  }
}

void page_test(){
  char ct[20];
  uint64_t *test_ttbr = (uint64_t*)malloc(PAGE_SIZE, 1);
  int_to_hex((uint64_t)test_ttbr, ct);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  
  
  int64_t first_user_addr = page_action_rec(0x8000, (struct pg_t*)test_ttbr, 0, VtoP_CREAT);
  int_to_hex(first_user_addr, ct);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  
  first_user_addr = page_action_rec(0x8f00, (struct pg_t*)test_ttbr, 0, VtoP_CREAT);
  int_to_hex(first_user_addr, ct);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  
  first_user_addr = page_action_rec(0x10f00, (struct pg_t*)test_ttbr, 0, VtoP_CREAT);
  int_to_hex(first_user_addr, ct);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  user_pt_show(test_ttbr);
  
  int64_t r = page_action_rec(0x0, (struct pg_t*)test_ttbr, 0, RM);
  if (r <0) {
    log_puts("RM error\n", WARNING);
  }
  user_pt_show(test_ttbr);
  buddy_table_show();
  r = page_action_rec(0x8000, (struct pg_t*)test_ttbr, 0, RM);
  if (r <0) {
    log_puts("RM error\n", WARNING);
  }
  user_pt_show(test_ttbr);
  buddy_table_show();
  r = page_action_rec(0x0, (struct pg_t*)test_ttbr, 0, RM_ALL);
  user_pt_show(test_ttbr);
  buddy_table_show();
}

