#include "bootloader.h"
#include "uart.h"
#include "utils.h"

#define BUDDY_START 0x10000000
#define BUDDY_END (BUDDY_START + 1024 * PAGE_SIZE)
// #define BUDDY_END 0x40000000
#define CPIO_ARRD 0x8000000

#define PAGE_SIZE (4 * KB)
#define BUDDY_ARRAY_SIZE ((BUDDY_END - BUDDY_START) / PAGE_SIZE)
#define BUDDY_INDEX 11
#define BUDDY_LEN 16

#define CPIO_MAGIC_BYTES 6
#define CPIO_OTHERS_BYTES 8
#define CPIO_SIZE (CPIO_MAGIC_BYTES + CPIO_OTHERS_BYTES * 13)

struct cpio_newc_header {
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
};

int cpio_info(struct cpio_newc_header **cpio_ptr, char **cpio_addr,
              char **context) {
  *cpio_ptr = (struct cpio_newc_header *)(*cpio_addr);

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

struct buddy_list {
  struct buddy_list *next;
  char *addr;
  int size;
};
struct buddy_list buddy[BUDDY_ARRAY_SIZE];
struct buddy_list *free_list[BUDDY_LEN];
struct buddy_list *used_list[BUDDY_LEN];

void buddy_init(char *mem_start) {
  for (int i = 0; i < BUDDY_INDEX; i++) free_list[i] = 0;
  for (int i = 0; i < BUDDY_INDEX; i++) used_list[i] = 0;
  for (int i = 0; i < BUDDY_ARRAY_SIZE; i++) {
    buddy[i].next = 0;
    buddy[i].addr = mem_start + i * PAGE_SIZE;
    buddy[i].size = -1;
  }

  for (int size = 4 * MB / PAGE_SIZE, i = 0; size > 0; size >>= 1)
    for (; i + size <= BUDDY_ARRAY_SIZE; i += size) {
      if (i + 2 * size <= BUDDY_ARRAY_SIZE) buddy[i].next = &buddy[i + size];
      buddy[i].size = size;
    }

  for (int size = 4 * MB / PAGE_SIZE, reminder_size = BUDDY_ARRAY_SIZE, i = 0;
       size > 0; size >>= 1)
    if (buddy[i].size == size) {
      free_list[size2Index(size)] = &buddy[i];
      i += (reminder_size / size) * size;
      reminder_size %= size;
    }
}
struct buddy_list *buddy_alloc(int size) {
  size /= PAGE_SIZE;
  if (free_list[size2Index(size)] == 0) {
    struct buddy_list *now = buddy_alloc(2 * size * PAGE_SIZE);
    used_list[size2Index(2 * size)] = now->next;
    free_list[size2Index(size)] = now;
    now->size = size;
    now->next = now + now->size;
    now = now->next;
    now->size = size;
    now->next = 0;
  }
  struct buddy_list *now = free_list[size2Index(size)];
  free_list[size2Index(size)] = now->next;
  now->next = used_list[size2Index(size)];
  used_list[size2Index(size)] = now;
  return now;
}
struct buddy_list *buddy_merge(struct buddy_list *now, struct buddy_list *next,
                               struct buddy_list **before) {
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
void buddy_free(struct buddy_list *list) {
  int size = list->size;
  struct buddy_list *now = used_list[size2Index(size)],
                    **before = &used_list[size2Index(size)];
  for (; now; before = &now->next, now = now->next)
    if (now == list) break;
  *before = list->next;
  now = free_list[size2Index(size)];
  before = &free_list[size2Index(size)];
  struct buddy_list *merge;
  for (; now; before = &now->next, now = now->next) {
    if ((merge = buddy_merge(list, now, before))) {
      buddy_free(merge);
      return;
    }
  }

  list->next = free_list[size2Index(size)];
  free_list[size2Index(size)] = list;
}
void print_buddyList(struct buddy_list **lists) {
  for (int i = 0; i < BUDDY_INDEX; i++) {
    uart_puts("index: ");
    print_h((unsigned long int)i);
    for (struct buddy_list *now = lists[i]; now != 0; now = now->next) {
      uart_puts("    allocate memory addr: ");
      print_h((unsigned long int)now->addr);
      // uart_puts("        allocate memory addr end: ");
      // print_h((unsigned long int)now->addr + now->size * 4 * KB);
    }
  }
  uart_puts("buddy end addr: ");
  print_h((unsigned long int)BUDDY_END);
}

void buddy_test1() {
  uart_puts("\r\nbuddy_test1\r\n");
  struct buddy_list *list0 = buddy_alloc(PAGE_SIZE),
                    *list1 = buddy_alloc(PAGE_SIZE),
                    *list2 = buddy_alloc(2 * PAGE_SIZE);
  // *list3 = buddy_alloc(PAGE_SIZE);
  print_buddyList(free_list);
  uart_puts("\r\n");
  print_buddyList(used_list);
  uart_puts("\r\n++++++++++++++++++++++++++++++\r\n");
  buddy_free(list1);
  buddy_free(list2);
  buddy_free(list0);
  uart_puts("\r\n");
  print_buddyList(free_list);
  uart_puts("\r\n");
  print_buddyList(used_list);
  uart_puts("\r\nending\r\n");
  return;
}

void buddy_test2() {
  uart_puts("\r\nbuddy_test2\r\n");
  struct buddy_list *list0 = buddy_alloc(PAGE_SIZE),
                    *list1 = buddy_alloc(PAGE_SIZE),
                    *list2 = buddy_alloc(PAGE_SIZE),
                    *list3 = buddy_alloc(PAGE_SIZE);
  print_buddyList(free_list);
  uart_puts("\r\n");
  print_buddyList(used_list);
  uart_puts("\r\n++++++++++++++++++++++++++++++\r\n");
  buddy_free(list1);
  buddy_free(list2);
  buddy_free(list0);
  buddy_free(list3);
  uart_puts("\r\n");
  print_buddyList(free_list);
  uart_puts("\r\n");
  print_buddyList(used_list);
  uart_puts("\r\nending\r\n");
  return;
}