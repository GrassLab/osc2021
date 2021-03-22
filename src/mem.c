#include "mem.h"

#include <stddef.h>

#include "io.h"
#include "util.h"

#define get_order(ptr) (__builtin_ctzl((unsigned long)ptr) - PAGE_SIZE_CTZ)
#define set_buddy_ord(bd, ord) (bd = ord | (bd & 0xc0))
#define set_buddy_flag(bd, flag) (bd = (flag << 6) | (bd & 0x3f))
#define get_buddy_ord(bd) (bd & 0x3f)
#define get_buddy_flag(bd) ((bd & 0xc0) >> 6)
#define ptr_to_pagenum(ptr) (((unsigned long)(ptr)) >> PAGE_SIZE_CTZ)
#define pagenum_to_ptr(pn) ((void *)((pn) << PAGE_SIZE_CTZ))
#define buddy_pagenum(pg, ord) ((pg) ^ (1 << ord))
#define BUDDY_FREE 0
#define BUDDY_USE 1
#define SLAB_USE 2
#define RESRVE_USE 3

typedef struct buddy_system {
  list_head free_list[BUDDY_MAX];
} buddy_system;

typedef struct startup_allocator {
  void *addr[STARTUP_MAX];
  unsigned long long size[STARTUP_MAX];
} startup_allocator;

// typedef struct smallbin {
//   list_head free_list[SMALLBIN_SiZE];
// } smallbin;

// typedef struct middlebin {
//   list_head free_list[MIDDLEBIN_SIZE];
// } middlebin;

unsigned long long mem_size = 0x40000000;  // 1 GB
unsigned long long reserve_count = 0;
buddy_system bs;
startup_allocator sa;
char *buddy_stat;

// smallbin sb;
// middlebin mb;
// list_head lb;
// void *heap_arena;

// list_head *chunk2list(void *chunk) {
//   return (list_head *)((chunk) + 0x10);
// }

// void *list2chunk(list_head *l) { return ((void *)l - 0x10); }

// size_t chunkprevuse(void *chunk) {
//   return (*(unsigned long long *)((chunk) + 0x8) & 1);
// }

// void setchunksize(void *chunk, size_t size) {
//   *(unsigned long long *)((chunk) + 0x8) = (size) | chunkprevuse(chunk);
// }

// size_t getchunksize(void *chunk) {
//   return (*(unsigned long long *)((chunk) + 0x8) / 2) * 2;
// }

// void setprevuse(void *chunk) {
//   *(unsigned long long *)((chunk) + 0x8) |= 1;
// }

// void setprevfree(void *chunk) {
//   *(unsigned long long *)((chunk) + 0x8) = getchunksize(chunk);
// }

// void setprevsize(void *chunk, size_t size) {
//   *(unsigned long long *)(chunk) = size;
// }

// void *nextchunk(void *chunk) { return chunk + getchunksize(chunk); }

// int in_heap(void *chunk) {
//   void *itr_arena = heap_arena;
//   while (itr_arena != NULL) {
//     if (chunk >= itr_arena &&
//         chunk < (itr_arena + *(unsigned long long *)(itr_arena + 0x8))) {
//       return 1;
//     }
//     itr_arena = *(void **)(itr_arena + 0x10);
//   }
//   return 0;
// }

void *kmalloc(unsigned long size) {
  size = pad(size, 8);
  if (size > PAGE_SIZE / 2) {
    return alloc_page(size);
  } else {
    // return alloc_heap(size);
    return NULL;
  }
}

void kfree(void *ptr) {
  int flag = get_buddy_flag(buddy_stat[ptr_to_pagenum(ptr)]);
  if (flag == BUDDY_USE) {
    free_page(ptr);
  } else if (flag == RESRVE_USE) {
    free_reserve(ptr);
  }
  // if (in_heap(ptr)) {
  //   free_heap(ptr);
  // } else {
  //   free_page(ptr);
  // }
}

void sort_reserve() {
  for (int i = reserve_count - 1; i >= 0; i--) {
    for (int j = 0; j < i; j++) {
      if ((unsigned long long)sa.addr[j] > (unsigned long long)sa.addr[j + 1]) {
        void *tmp_addr = sa.addr[j];
        sa.addr[j] = sa.addr[j + 1];
        sa.addr[j + 1] = tmp_addr;
        unsigned long long tmp_size = sa.size[j];
        sa.size[j] = sa.size[j + 1];
        sa.size[j + 1] = tmp_size;
      }
    }
  }
}

int check_reserve_collision(void *a1, size_t s1, void *a2, size_t s2) {
  void *e1 = (void *)((unsigned long long)a1 + s1);
  void *e2 = (void *)((unsigned long long)a2 + s2);
  return ((a2 >= a1) && (a2 < e1)) || ((e2 > a1) && (e2 <= e1)) ||
         ((a1 >= a2) && (a1 < e2)) || ((e1 > a2) && (e1 <= e2));
}

int reserve_mem(void *addr, unsigned long size) {
  if (((unsigned long long)addr & 0xfff) != 0 || (size & 0xfff) != 0) {
    log("reserve mem require page align\n");
    return -1;
  }
  if (reserve_count >= STARTUP_MAX) {
    log("no reserve slot available\n");
    return -1;
  } else {
    for (int i = 0; i < reserve_count; i++) {
      if (check_reserve_collision(sa.addr[i], sa.size[i], addr, size)) {
        log("reserve collision\n");
        return -1;
      }
    }
    sa.addr[reserve_count] = addr;
    sa.size[reserve_count] = size;
    reserve_count++;
    return 0;
  }
}

void _place_buddy(void *ptr, int ord, int flag) {
  unsigned long long idx = (unsigned long long)ptr >> PAGE_SIZE_CTZ;

  set_buddy_ord(buddy_stat[idx], ord);
  set_buddy_flag(buddy_stat[idx], flag);

  if (flag == BUDDY_FREE) {
    push_list(&bs.free_list[ord], (list_head *)ptr);
  }
}

void place_buddy(void *st, void *ed, int flag) {
  while (st != ed) {
    int st_ord = get_order(st);
    int ed_ord = get_order(ed);
    if (st_ord >= BUDDY_MAX) {
      st_ord = BUDDY_MAX - 1;
    }
    if (ed_ord >= BUDDY_MAX) {
      ed_ord = BUDDY_MAX - 1;
    }
    if (st_ord <= ed_ord) {
      _place_buddy(st, st_ord, flag);
      st += (1 << (st_ord + PAGE_SIZE_CTZ));
    } else {
      ed -= (1 << (ed_ord + PAGE_SIZE_CTZ));
      _place_buddy(ed, ed_ord, flag);
    }
  }
}

void init_buddy(char *stat_ptr) {
  buddy_stat = stat_ptr;
  sort_reserve();
  for (int i = 0; i < BUDDY_MAX; i++) {
    init_list(&(bs.free_list[i]));
  }
  for (unsigned long i = 0; i < mem_size / PAGE_SIZE; i++) {
    buddy_stat[i] = 0x40;
  }
  void *mem_itr = 0;
  for (int i = 0; i < reserve_count; i++) {
    place_buddy(mem_itr, sa.addr[i], BUDDY_FREE);
    place_buddy(sa.addr[i], sa.addr[i] + sa.size[i], RESRVE_USE);
    mem_itr = sa.addr[i] + sa.size[i];
  }
  place_buddy(mem_itr, (void *)mem_size, BUDDY_FREE);
}

// void init_heap(void *heap_start, size_t heap_size) {
//   for (int i = 0; i < SMALLBIN_SiZE; i++) {
//     init_list(&sb.free_list[i]);
//   }
//   for (int i = 0; i < MIDDLEBIN_SIZE; i++) {
//     init_list(&mb.free_list[i]);
//   }
//   init_list(&lb);

//   heap_arena = heap_start;
//   *(void **)heap_start = heap_start;
//   *(unsigned long long *)(heap_start + 0x8) = heap_size;
//   *(void **)(heap_start + 0x10) = NULL;
//   void *chunk_start = heap_start + 0x10;

//   push_list(&lb, chunk2list(chunk_start));
//   setchunksize(chunk_start, heap_size - 0x20);
//   setprevuse(chunk_start);
//   setchunksize(nextchunk(chunk_start), 0);
//   setprevfree(nextchunk(chunk_start));
// }

void *alloc_page(size_t size) {
  size = pad(size, PAGE_SIZE);
  return NULL;
}

// void *alloc_heap(size_t size) {
//   size = pad(size + 8, 16);
//   return NULL;
// }

void free_page(void *ptr) {
  log("free page: ");
  log_hex((unsigned long long)ptr);
  log("\n");
  unsigned long pagenum = ptr_to_pagenum(ptr);
  unsigned long ord = get_buddy_ord(buddy_stat[pagenum]);
  buddy_stat[pagenum] = 0x40;

  while (ord < BUDDY_MAX - 1) {
    unsigned long buddy = buddy_pagenum(pagenum, ord);
    if (get_buddy_flag(buddy_stat[buddy]) == BUDDY_FREE &&
        get_buddy_ord(buddy_stat[buddy]) == ord) {
      log("coalesce\n");
      pop_list((list_head *)pagenum_to_ptr(buddy));
      buddy_stat[buddy] = 0x40;
      ord++;
      pagenum = pagenum < buddy ? pagenum : buddy;
    } else {
      break;
    }
  }
  set_buddy_flag(buddy_stat[pagenum], BUDDY_FREE);
  set_buddy_ord(buddy_stat[pagenum], ord);
  push_list(&bs.free_list[ord], pagenum_to_ptr(pagenum));
}

// void free_heap(void *ptr) { log("free heap\n"); }

void free_reserve(void *ptr) {
  log("free reserve: ");
  log_hex((unsigned long long)ptr);
  log("\n");
  void *st = ptr;
  void *ed;
  for (int i = 0; i < reserve_count; i++) {
    if (sa.addr[i] == st) {
      ed = st + sa.size[i];
    }
  }

  while (st != ed) {
    int st_ord = get_order(st);
    int ed_ord = get_order(ed);
    if (st_ord >= BUDDY_MAX) {
      st_ord = BUDDY_MAX - 1;
    }
    if (ed_ord >= BUDDY_MAX) {
      ed_ord = BUDDY_MAX - 1;
    }
    if (st_ord <= ed_ord) {
      free_page(st);
      st += (1 << (st_ord + PAGE_SIZE_CTZ));
    } else {
      ed -= (1 << (ed_ord + PAGE_SIZE_CTZ));
      free_page(ed);
    }
  }
}

void log_buddy() {
  for (int i = 0; i < BUDDY_MAX; i++) {
    list_head *l = &bs.free_list[i];
    list_head *head = l;
    int count = 0;
    log("ord: ");
    log_hex(i);
    log("\n");
    while (l->fd != head) {
      log("block: ");
      log_hex((unsigned long long)l->fd);
      log("\n");
      l = l->fd;
      count++;
    }
    log("total: ");
    log_hex(count);
    log("\n");
  }
}