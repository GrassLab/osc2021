# ifndef __PAGE_H__
# define __PAGE_H__

# define PD_TABLE            0b11
# define PD_BLOCK            0b01
# define PD_PAGE             0b11
# define PD_ACCESS           (1 << 10)
# define PD_USER             (1 << 6)

# define MAIR_DEVICE_nGnRnE        0b00000000
# define MAIR_NORMAL_NOCACHE       0b01000100
# define MAIR_IDX_DEVICE_nGnRnE    0
# define MAIR_IDX_NORMAL_NOCACHE   1
# define MAIR_EL1_VALUE          ((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
                                 (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))
# define PAGE_SIZE       0x1000
# define PAGE_SIZE_BITS  12
# define PD_LEN          (PAGE_SIZE/8)
# define PD_LEN_BITS     9

# define PGD_ATTR        PD_TABLE
# define PUD0_ATTR       PD_TABLE
# define PUD1_ATTR       (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
# define PME_ATTR        PD_TABLE
# define PTE_DEVICE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)
# define PTE_NORMAL_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)

# define PGE_USER_ATTR   PD_TABLE
# define PUE_USER_ATTR   PD_TABLE
# define PME_USER_ATTR   PD_TABLE
# define PXE_USER_ATTR   PD_TABLE
# define PTE_USER_ATTR   PTE_NORMAL_ATTR | PD_USER
//# define PTE_USER_ATTR   PTE_NORMAL_ATTR

# ifndef __ASM_S__

# include "typedef.h"
# include "list.h"

# define PG_R_CREAT     1

enum page_act{
  VtoP = 0,
  VtoP_CREAT = 1,
  GET_ALL = 2,
  RM = 3,
  RM_ALL = 4,
};

struct pg_list{
  uint64_t va;
  uint64_t pa;
  list_head head;
};

struct pg_t{
  uint64_t pg_data[PD_LEN];
};

void page_test();

int64_t create_user_page(uint64_t v_addr, uint64_t ttbr0);
int64_t rmall_user_page(uint64_t ttbr0);
uint64_t get_kernel_ttbr0();
void user_pt_show(void* ttbr);

extern "C" void kernel_page_setup();
# endif
# endif
