#include "../include/memAlloc.h"
#include "../include/uart.h"

#define PAGE_SIZE 4096
#define MEM_SIZE 0x10000000 // 0.25G
#define MEM_START 0x10000000
#define PAGE_NUM (MEM_SIZE / PAGE_SIZE)
#define CHUNK16_NUM 4096 / 16
#define CHUNK32_NUM 4096 / 32
#define CHUNK64_NUM 4096 / 64

static node_info page_arr[PAGE_NUM];
static list_node list_node_arr[17];
static list_node pool[65536];
static int used = 0;
static chunk_info chunk16[CHUNK16_NUM];
static chunk_info chunk32[CHUNK32_NUM];
static chunk_info chunk64[CHUNK64_NUM];
static int addr16;
static int addr32;
static int addr64;

// enum chunk_size : long {
//    cs_16 = 0,
//    cs_32,
//    cs_64,
//    cs_size
//};

void list_node_init() {

  for (int i = 0; i < 17; ++i) {
    list_node_arr[i].next = &list_node_arr[i];
    list_node_arr[i].prev = &list_node_arr[i];
  }

  list_node_push(&pool[used], 16);
  pool[used].buddy_index = -1;
  pool[used].start_addr = 0x10000000;
  pool[used].fr_no = 0;
  used++;
}

int check_list(int index) {
  if (list_node_arr[index].next == &list_node_arr[index]) {
    return false;
  } else {
    int address = list_node_arr[index].next->start_addr;
    return address;
  }
}

void list_node_pop(int exp) {

  // list_node* temp = list_node_arr[exp].next;
  // list_node_arr[exp].next = list_node_arr[exp].next -> next;
  // list_node_arr[exp].prev -> prev = list_node_arr[exp].next -> prev;
  // temp -> next = NULL;
  // temp -> prev = NULL;
  list_node_del(list_node_arr[exp].next);
}

void list_node_push(list_node *entry, int exp) {

  entry->next = list_node_arr[exp].next;
  entry->prev = list_node_arr[exp].next->prev;
  list_node_arr[exp].next->prev = entry;
  list_node_arr[exp].next = entry;
}

void list_node_del(list_node *entry) {
  // uart_printf("%x\n",entry->next -> start_addr);
  // if(entry->prev == &list_node_arr[15]){
  //    uart_puts("test\n");
  //}
  entry->prev->next = entry->next;
  entry->next->prev = entry->prev;
  entry->next = NULL;
  entry->prev = NULL;
}

void dy_mem_status_dump() {
  uart_puts("size 16:");
  for (int i = 0; i < CHUNK16_NUM; ++i) {
    uart_printf("%d ", chunk16[i].onused);
  }
  uart_puts("\n");
  uart_puts("size 32:");
  for (int i = 0; i < CHUNK32_NUM; ++i) {
    uart_printf("%d ", chunk32[i].onused);
  }
  uart_puts("\n");
  uart_puts("size 64:");
  for (int i = 0; i < CHUNK64_NUM; ++i) {
    uart_printf("%d ", chunk64[i].onused);
  }
  uart_puts("\n");
}

void mem_status_dump() {

  for (int i = 16; i >= 0; --i) {
    int max_item = 17 - i;
    int iter = 0;
    uart_printf("size 2^%d \n", i + 12);
    list_node *temp = list_node_arr[i].next;
    while (temp != &list_node_arr[i] && iter < max_item) {
      uart_printf("start at 0x%x ", temp->start_addr);
      temp = temp->next;
      iter++;
    }
    uart_printf("\n");
  }
}

void split_node(int exp) {

  uart_printf("split from size 2^%d to 2^%d\n", exp + 12, exp + 11);
  int addr1, addr2;
  addr1 = list_node_arr[exp].next->start_addr;
  addr2 = (addr1 + (1 << (exp + 11)));

  int fr_no1, fr_no2;
  fr_no1 = list_node_arr[exp].next->fr_no;
  fr_no2 = fr_no1 + (1 << (exp - 1));
  list_node_pop(exp);
  pool[used].start_addr = addr1;
  pool[used].fr_no = fr_no1;
  pool[used].buddy_index = fr_no2;

  int page_no = (addr1 - MEM_START) >> 12;
  int frame_size = 1 << (exp - 1);
  while (frame_size > 0) {
    page_arr[page_no].corespond_list_node = &pool[used];
    page_arr[page_no].buddy = fr_no2;
    page_arr[page_no].exp = exp - 1;
    page_no++;
    frame_size--;
  }

  list_node_push(&pool[used], exp - 1);
  used++;
  pool[used].start_addr = addr2;
  pool[used].fr_no = fr_no2;
  pool[used].buddy_index = fr_no1;

  page_no = (addr2 - MEM_START) >> 12;
  frame_size = 1 << (exp - 1);
  while (frame_size > 0) {
    page_arr[page_no].corespond_list_node = &pool[used];
    page_arr[page_no].buddy = fr_no1;
    page_arr[page_no].exp = exp - 1;
    page_no++;
    frame_size--;
  }
  list_node_push(&pool[used], exp - 1);
  used++;
}

int find_space(int exp) {

  int address = check_list(exp - 12);
  if (address == 0) {
    uart_printf("Currently no desired size frame, need to split\n");
    return NULL;
  } else {
    uart_printf("found contiguous space of 2^%d at address:0x%x\n", exp,
                address);
    return address;
  }
}

void *my_alloc(int size) {
  int dy_allocated = 0;
  if (size <= 96) {
    int exp = 4;
    while ((1 << exp) < size) {
      ++exp;
    }
    int dy_addr = dy_alloc(1 << exp);
    if (dy_addr != 0) {
      uart_printf("%x\n", dy_addr);
      dy_allocated = 1;
      return dy_addr;
    }
  }

  if (!dy_allocated) {
    // find the 4KB roundup
    int exp = 12;
    while ((1 << exp) < size) {
      ++exp;
    }

    int address = find_space(exp);

    int done = 0;
    while (!check_list(exp - 12)) {
      for (int i = exp - 11; i < 17; ++i) {
        if (check_list(i)) {
          split_node(i);
          break;
        }
      }
    }
    address = find_space(exp);
    int page_no = (address - MEM_START) >> 12;
    int frame_size = (1 << (exp - 12));
    while (frame_size > 0) {
      page_arr[page_no].inused = 1;
      page_no++;
      frame_size--;
    }
    list_node_pop(exp - 12);
    return address;
  }
}

void my_free(void *addr) {

  if ((int)addr >= addr16 && (int)addr < (addr16 + PAGE_SIZE)) {
    chunk16[((int)addr - addr16) >> 4].onused = 0;
    return;
  } else if ((int)addr >= addr32 && (int)addr < (addr32 + PAGE_SIZE)) {
    chunk32[((int)addr - addr32) >> 5].onused = 0;
    uart_printf("%d\n", ((int)addr - addr32) >> 5);
    return;
  } else if ((int)addr >= addr64 && (int)addr < (addr64 + PAGE_SIZE)) {
    chunk64[((int)addr - addr64) >> 6].onused = 0;
    return;
  }
  int page_no = ((int)addr - MEM_START) >> 12;
  int my_head = page_no;
  int exp = page_arr[page_no].exp;
  int frame_size = 1 << exp;
  int buddy = page_no ^ (1 << (exp)); // page_arr[page_no].buddy;
  int buddy_head = buddy;
  list_node *freed = page_arr[page_no].corespond_list_node;

  list_node_push(freed, exp);
  int free;
  for (int i = 0; i < frame_size; ++i) {
    page_arr[page_no++].inused = 0;
  }
  free = 1;
  while (free == 1 && exp < 16) {
    for (int i = 0; i < frame_size; ++i) {
      if (page_arr[buddy++].inused == 1) {
        free = 0;
      }
    }

    if (free == 1) {
      buddy = buddy_head;
      uart_printf("merge 2 size 2^%d\n", exp + 12);

      pool[used].fr_no = page_no < buddy ? page_no : buddy;
      pool[used].start_addr = (pool[used].fr_no << 12) + MEM_START;
      pool[used].buddy_index = pool[used].fr_no ^ (1 << (exp + 1));
      list_node_push(&pool[used], exp + 1);

      list_node_del(freed);
      list_node_del(page_arr[buddy_head].corespond_list_node);

      for (int i = 0; i < frame_size; ++i) {
        page_arr[my_head].exp = exp + 1;
        page_arr[my_head].corespond_list_node = &pool[used];
        my_head++;
      }
      for (int i = 0; i < frame_size; ++i) {
        page_arr[buddy].exp = exp + 1;
        page_arr[buddy].corespond_list_node = &pool[used];
        buddy++;
      }

      my_head = pool[used].fr_no;
      buddy = pool[used].buddy_index;
      buddy_head = buddy;
      page_no = pool[used].fr_no;
      frame_size = 1 << (exp + 1);
      freed = &pool[used];
      used++;
    }
    exp++;
  }
}

void *dy_alloc(int size) {

  if (size == 16) {
    for (int i = 0; i < CHUNK16_NUM; ++i) {
      if (chunk16[i].onused == 0) {
        chunk16[i].onused = 1;
        return addr16 + (i << 4);
      }
    }
  } else if (size == 32) {
    for (int i = 0; i < CHUNK32_NUM; ++i) {
      if (chunk32[i].onused == 0) {
        chunk32[i].onused = 1;
        return addr32 + (i << 5);
      }
    }
  } else if (size == 64) {
    for (int i = 0; i < CHUNK64_NUM; ++i) {
      if (chunk64[i].onused == 0) {
        chunk64[i].onused = 1;
        return addr64 + (i << 6);
      }
    }
  }
  return 0;
}
int mem_init() {

  list_node_init();

  for (int i = 0; i < PAGE_NUM; ++i) {
    page_arr[i].inused = 0;
    page_arr[i].buddy = -1;
    page_arr[i].exp = -1;
  }
  addr16 = my_alloc(4096);
  addr32 = my_alloc(4096);
  addr64 = my_alloc(4096);
  for (int i = 0; i < CHUNK16_NUM; ++i) {
    chunk16[i].onused = 0;
  }
  for (int i = 0; i < CHUNK32_NUM; ++i) {
    chunk32[i].onused = 0;
  }
  for (int i = 0; i < CHUNK64_NUM; ++i) {
    chunk64[i].onused = 0;
  }
  //    for (int i = 0; i < cs_size; ++i) {
  //        chunk_addr[i] = my_alloc(4096);
  //    }
  //    chunk_addr[cs_32]
  return 0;
}
