#include "bootloader.h"
#include "uart.h"
#include "utils.h"

void pause() {
  uart_puts("Press any key to continue . . .");
  uart_getc();
  uart_puts("\r                                \r");
}
/* cpio */
#define CPIO_ARRD 0x8000000
#define CPIO_MAGIC_BYTES 6
#define CPIO_OTHERS_BYTES 8
#define CPIO_SIZE (CPIO_MAGIC_BYTES + CPIO_OTHERS_BYTES * 13)

typedef struct cpio_newc_header {
  char c_magic[6];
  char c_ino[8];
  char c_mode[8];
  char c_uid[8];
  char c_gid[8];
  char c_nlink[8];
  char c_mtime[8];
  char c_filesize[8];
  char c_devmajor[8];
  char c_devminor[8];
  char c_rdevmajor[8];
  char c_rdevminor[8];
  char c_namesize[8];
  char c_check[8];
} cpio_newc_header;

int cpio_info(cpio_newc_header **cpio_ptr, char **cpio_addr, char **context) {
  *cpio_ptr = (cpio_newc_header *)(*cpio_addr);

  /* get filename size */
  unsigned long long int c_namesize =
      atoHex_size((*cpio_ptr)->c_namesize, CPIO_OTHERS_BYTES);

  /* get cpio header size */
  unsigned long long int header_size =
      CPIO_SIZE + c_namesize + align(CPIO_SIZE + c_namesize, 4);

  /* get cpio context size */
  unsigned long long int context_size =
      atoHex_size((*cpio_ptr)->c_filesize, CPIO_OTHERS_BYTES);
  unsigned long long int size = context_size + align(context_size, 4);
  *context = (*cpio_addr) + header_size;

  /* move to next cpio header */
  *cpio_addr += header_size + size;
  return context_size;
}

/* buddy system */
#define BUDDY_START 0x10000000
#define BUDDY_END 0x20000000
#define PAGE_SIZE (4 * KB)
#define BUDDY_ARRAY_SIZE ((BUDDY_END - BUDDY_START) / PAGE_SIZE)
#define BUDDY_INDEX 20

typedef struct buddy_list {
  struct buddy_list *next;
  void *addr;
  int size;
} buddy_list;
buddy_list buddy[BUDDY_ARRAY_SIZE];
buddy_list *free_list[BUDDY_INDEX];
buddy_list *used_list[BUDDY_INDEX];

void buddy_init(void *mem_start) {
  for (int i = 0; i < BUDDY_INDEX; i++) free_list[i] = used_list[i] = 0;
  for (int i = 0; i < BUDDY_ARRAY_SIZE; i++)
    buddy[i] =
        (buddy_list){.next = 0, .addr = mem_start + i * PAGE_SIZE, .size = -1};
  int begin_size = 2 << (BUDDY_INDEX - 1);
  for (int size = begin_size, i = 0; size > 0; size >>= 1)
    for (; i + size <= BUDDY_ARRAY_SIZE; i += size) {
      if (i + 2 * size <= BUDDY_ARRAY_SIZE) buddy[i].next = &buddy[i + size];
      buddy[i].size = size;
    }
  for (int size = begin_size, reminder_size = BUDDY_ARRAY_SIZE, i = 0; size > 0;
       size >>= 1)
    if (buddy[i].size == size) {
      free_list[size2Index(size)] = &buddy[i];
      i += (reminder_size / size) * size;
      reminder_size %= size;
    }
}
buddy_list *buddy_alloc(int size) {
  size /= PAGE_SIZE;
  if (free_list[size2Index(size)] == 0) {
    buddy_list *now = buddy_alloc(2 * size * PAGE_SIZE);
    used_list[size2Index(2 * size)] = now->next;
    free_list[size2Index(size)] = now;
    now->size = size;
    now->next = now + now->size;
    now = now->next;
    now->size = size;
    now->next = 0;
  }
  buddy_list *now = free_list[size2Index(size)];
  free_list[size2Index(size)] = now->next;
  now->next = used_list[size2Index(size)];
  used_list[size2Index(size)] = now;
  return now;
}
buddy_list *__buddy_merge__(buddy_list *now, buddy_list *next,
                            buddy_list **before) {
  int size = now->size;
  if (now > next) swap((void *)&now, (void *)&next);
  if (now + size != next ||
      ((uint64_t)now->addr - BUDDY_START) / PAGE_SIZE % (size << 1) != 0)
    return 0;
  *before = next->next;
  next->size = -1;
  next->next = 0;
  now->size <<= 1;
  now->next = 0;
  return now;
}
void buddy_free(buddy_list *list) {
  int size = list->size;
  buddy_list *now = used_list[size2Index(size)],
             **before = &used_list[size2Index(size)];
  for (; now; before = &now->next, now = now->next)
    if (now == list) break;
  *before = list->next;
  now = free_list[size2Index(size)];
  before = &free_list[size2Index(size)];
  buddy_list *merge;
  for (; now; before = &now->next, now = now->next) {
    if ((merge = __buddy_merge__(list, now, before))) {
      buddy_free(merge);
      return;
    }
  }

  list->next = free_list[size2Index(size)];
  free_list[size2Index(size)] = list;
}

void print_buddyList(buddy_list **lists) {
  for (int i = 0; i < BUDDY_INDEX; i++) {
    if (lists[i] != 0) {
      uart_puts("index: ");
      print_h((unsigned long int)i);
      for (buddy_list *now = lists[i]; now != 0; now = now->next) {
        uart_puts("    memory addr: ");
        print_h((unsigned long int)now->addr);
        // uart_puts("        allocate memory addr end: ");
        // print_h((unsigned long int)now->addr + now->size * 4 * KB);
      }
      uart_puts("\r\n");
    }
  }
}
/* dma */
typedef struct dma {
  struct dma *next;
  buddy_list *page;
  int size;
} dma;
dma *dma_dummy[6];  // align for 8
dma *free_pool, *used_pool;
void dma_init() { free_pool = used_pool = 0; }

void *malloc(int size) {
  size += align(size, 8);
  int dmaSize = sizeof(dma) + align(sizeof(dma), 8);
  /* get appropriate size*/
  int min = GB;
  dma *result = 0, **rBefore = &free_pool;
  for (dma *now = free_pool, **before = &free_pool; now;
       before = &now->next, now = now->next) {
    int delta = now->size - dmaSize - size;
    if (0 < delta && delta < min) {
      min = delta;
      result = now;
      rBefore = before;
    }
  }
  if (result == 0) {
    int buddy_size = (2 * dmaSize + size) / PAGE_SIZE;
    buddy_size =
        PAGE_SIZE << ((buddy_size == 0) ? 0 : (size2Index(buddy_size) + 1));
    buddy_list *page = buddy_alloc(buddy_size);
    result = (dma *)page->addr;
    result->size = buddy_size - dmaSize;
    result->page = page;
    result->next = free_pool;
  };
  dma *list = (dma *)((void *)result + dmaSize + size);
  list->next = result->next;
  list->page = result->page;
  list->size = result->size - dmaSize - size;
  *rBefore = list;

  /* alloc */
  result->next = used_pool;
  result->size = size;
  used_pool = result;
  return (void *)result + dmaSize;
}

void *__dma_merge__(dma *now, dma *next, dma **before) {
  if (now > next) swap((void *)&now, (void *)&next);
  int size = now->size, dmaSize = sizeof(dma) + align(sizeof(dma), 8);
  if ((void *)now + size + dmaSize != (void *)next || now->page != next->page)
    return 0;
  *before = (*before)->next;
  now->size += next->size + dmaSize;
  now->next = 0;
  return (void *)now + dmaSize;
}

void free(void *addr) {
  int dmaSize = sizeof(dma) + align(sizeof(dma), 8);
  dma *list = (dma *)(addr - dmaSize), *now = used_pool, **before = &used_pool;
  for (; now; before = &now->next, now = now->next)
    if (now == list) break;
  *before = list->next;
  now = free_pool;
  before = &free_pool;
  void *merge;
  for (; now; before = &now->next, now = now->next)
    if ((merge = __dma_merge__(list, now, before))) {
      free(merge);
      return;
    }
  if (list->size + dmaSize == list->page->size * PAGE_SIZE) {
    buddy_free(list->page);
    return;
  }
  list->next = free_pool;
  free_pool = list;
}
void printDmaPool(dma *list) {
  for (dma *now = list; now != 0; now = now->next) {
    uart_puts("addr: ");
    print_h((unsigned long int)((void *)now) + sizeof(dma));
    uart_puts("\tsize: ");
    print_h((unsigned long int)now->size);
    uart_puts("\r\n");
    // uart_puts("        allocate memory addr end: ");
    // print_h((unsigned long int)now->addr + now->size * 4 * KB);
  }
  uart_puts("pool end\r\n");
}
