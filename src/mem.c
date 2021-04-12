#include "mem.h"

#include "exc.h"
#include "io.h"
#include "util.h"

#define ptr_to_ord(ptr) (__builtin_ctzl((unsigned long)ptr) - PAGE_SIZE_CTZ)
#define flag_mask 0xe0
#define ord_mask 0x1f
#define set_page_ord(ps, ord) (ps = ord | (ps & flag_mask))
#define set_page_flag(ps, flag) (ps = flag | (ps & ord_mask))
#define get_page_ord(ps) (ps & ord_mask)
#define get_page_flag(ps) (ps & flag_mask)
#define ptr_to_pn(ptr) (((unsigned long)(ptr)) >> PAGE_SIZE_CTZ)
#define pn_to_ptr(pn) ((void *)((pn) << PAGE_SIZE_CTZ))
#define buddy_pn(pn, ord) ((pn) ^ (1 << ord))
#define size_to_idx(size) ((size / MEM_PAD) - 1)
#define idx_to_size(idx) ((idx + 1) * MEM_PAD)

#define BUDDY_FREE 0x0
#define BUDDY_USE 0x20
#define SLAB_USE 0x40
#define RESRVE_USE 0x60

typedef struct buddy_system {
  cdl_list free_list[BUDDY_MAX_ORD];
  unsigned char *page_stat;
} buddy_system;

typedef struct startup_allocator {
  void *addr[MEM_MAX_RESERVE];
  unsigned long size[MEM_MAX_RESERVE];
  unsigned long reserve_cnt;
} startup_allocator;

// at the head of page owned by slab
typedef struct page_descriptor {
  struct page_descriptor *next_pd;
  l_list free_list;
  unsigned long free_cnt;
  unsigned long slab_idx;
} page_descriptor;

typedef struct slab_cache {
  page_descriptor *head_pd;
  page_descriptor *cache_pd;
  void *slice_pos;
  unsigned long free_cnt;
  unsigned long remain;
  unsigned long pd_cnt;
} slab_cache;

#define SLAB_SIZE (PAGE_SIZE / 2 / MEM_PAD)

unsigned long mem_size = 0x40000000;  // 1 GB
slab_cache *slab[SLAB_SIZE];
startup_allocator sa = {.reserve_cnt = 0};
buddy_system bs;
slab_cache slab_slab;

void sort_reserve() {
  for (int i = sa.reserve_cnt - 1; i >= 0; i--) {
    for (int j = 0; j < i; j++) {
      if ((unsigned long)sa.addr[j] > (unsigned long)sa.addr[j + 1]) {
        void *tmp_addr = sa.addr[j];
        sa.addr[j] = sa.addr[j + 1];
        sa.addr[j + 1] = tmp_addr;
        unsigned long tmp_size = sa.size[j];
        sa.size[j] = sa.size[j + 1];
        sa.size[j + 1] = tmp_size;
      }
    }
  }
}

int check_collision(void *a1, size_t s1, void *a2, size_t s2) {
  void *e1 = (void *)((unsigned long long)a1 + s1);
  void *e2 = (void *)((unsigned long long)a2 + s2);
  return ((a2 >= a1) && (a2 < e1)) || ((e2 > a1) && (e2 <= e1)) ||
         ((a1 >= a2) && (a1 < e2)) || ((e1 > a2) && (e1 <= e2));
}

int reserve_mem(void *addr, unsigned long size) {
  if (((unsigned long)addr & 0xfff) != 0 || (size & 0xfff) != 0) {
    return RESERVE_NOT_ALIGN;
  }
  if (sa.reserve_cnt >= MEM_MAX_RESERVE) {
    return RESERVE_SLOT_FULL;
  } else {
    for (int i = 0; i < sa.reserve_cnt; i++) {
      if (check_collision(sa.addr[i], sa.size[i], addr, size)) {
        return RESERVE_COLLISION;
      }
    }
    sa.addr[sa.reserve_cnt] = addr;
    sa.size[sa.reserve_cnt] = size;
    sa.reserve_cnt++;
    return 0;
  }
}

void *reserve_alloc(unsigned long size) {
  size = pad(size, PAGE_SIZE);
  if (sa.reserve_cnt >= MEM_MAX_RESERVE) {
    log("reserve slot full", LOG_ERROR);
    return NULL;
  }
  if (reserve_mem((void *)0, size) != RESERVE_COLLISION) {
    return (void *)0;
  }
  for (int i = 0; i < sa.reserve_cnt; i++) {
    if (reserve_mem(sa.addr[i] + sa.size[i], size) != RESERVE_COLLISION) {
      return sa.addr[i] + sa.size[i];
    }
  }
  log("no reserve space", LOG_ERROR);
  return NULL;
}

// set page stat of page and push free list if BUDDY_FREE
void set_buddy(void *ptr, int ord, int flag) {
  unsigned long pn = ptr_to_pn(ptr);
  set_page_flag(bs.page_stat[pn], flag);
  set_page_ord(bs.page_stat[pn], ord);
  if (flag == BUDDY_FREE) {
    push_cdl_list(&(bs.free_list[ord]), (cdl_list *)ptr);
  }
}
// place memory segment into buddy system w/ flag
void place_buddy(void *st, void *ed, int flag) {
  while (st != ed) {
    int st_ord = ptr_to_ord(st);
    int ed_ord = ptr_to_ord(ed);
    st_ord = min(st_ord, BUDDY_MAX_ORD - 1);
    ed_ord = min(ed_ord, BUDDY_MAX_ORD - 1);

    if (st_ord <= ed_ord) {
      set_buddy(st, st_ord, flag);
      st += (1 << (st_ord + PAGE_SIZE_CTZ));
    } else {
      ed -= (1 << (ed_ord + PAGE_SIZE_CTZ));
      set_buddy(ed, ed_ord, flag);
    }
  }
}

void *alloc_page(unsigned long size) {
  size = pad(size, PAGE_SIZE);
  unsigned long ord = 51 - __builtin_clzl(size);
  if ((((1 << (ord + PAGE_SIZE_CTZ)) - 1) & size) != 0) {
    ord++;
  }
  log_hex("allocate page w/ order", ord, LOG_DEBUG);
  if (ord >= BUDDY_MAX_ORD) {
    log("out of memory", LOG_ERROR);
    return NULL;
  }
  unsigned long find_ord = ord;
  while (cdl_list_empty(&(bs.free_list[find_ord]))) {
    find_ord++;
    if (find_ord >= BUDDY_MAX_ORD) {
      log("alloc chunk larger than max ord", LOG_ERROR);
      return NULL;
    }
  }
  void *chunk = pop_cdl_list(bs.free_list[find_ord].fd);
  unsigned long pn = ptr_to_pn(chunk);
  set_page_flag(bs.page_stat[pn], BUDDY_USE);
  set_page_ord(bs.page_stat[pn], ord);
  while (find_ord > ord) {
    log_hex("release order", find_ord, LOG_DEBUG);
    find_ord--;
    unsigned long bd = buddy_pn(pn, find_ord);
    set_page_flag(bs.page_stat[bd], BUDDY_FREE);
    set_page_ord(bs.page_stat[bd], find_ord);
    push_cdl_list(&(bs.free_list[find_ord]), (cdl_list *)pn_to_ptr(bd));
  }
  return chunk;
}

void new_pd(unsigned long slab_idx) {
  log_hex("new pd at idx", slab_idx, LOG_DEBUG);
  unsigned long ord = slab[slab_idx]->pd_cnt;
  page_descriptor *pd = alloc_page(PAGE_SIZE << ord);
  unsigned long pn = ptr_to_pn(pd);
  set_page_flag(bs.page_stat[pn], SLAB_USE);
  for (int i = 1; i < (1 << ord); i++) {
    bs.page_stat[pn + i] = bs.page_stat[pn];
  }
  pd->free_cnt = 0;
  pd->free_list.next = NULL;
  pd->next_pd = slab[slab_idx]->head_pd;
  slab[slab_idx]->head_pd = pd;
  pd->slab_idx = slab_idx;
  slab[slab_idx]->slice_pos =
      (void *)((unsigned long)pd + pad(sizeof(page_descriptor), MEM_PAD));
  slab[slab_idx]->remain =
      (PAGE_SIZE << ord) - pad(sizeof(page_descriptor), MEM_PAD);
  slab[slab_idx]->pd_cnt++;
}

void *pop_slab_cache(slab_cache *sc) {
  if (sc->cache_pd->free_list.next == NULL) {
    page_descriptor *pd_itr = sc->head_pd;
    while (pd_itr->free_list.next == NULL) {
      pd_itr = pd_itr->next_pd;
    }
    sc->cache_pd = pd_itr;
  }
  page_descriptor *pd = sc->cache_pd;
  void *chunk = pop_l_list(&(pd->free_list));
  pd->free_cnt--;
  sc->free_cnt--;
  return chunk;
}

void *slice_slab_remain(unsigned long idx) {
  unsigned long size = idx_to_size(idx);
  void *chunk = slab[idx]->slice_pos;
  slab[idx]->remain -= size;
  slab[idx]->slice_pos += size;
  return chunk;
}

void *alloc_slab(unsigned long size) {
  unsigned long idx = size_to_idx(size);
  log_hex("allocate slab at idx", idx, LOG_DEBUG);
  if (slab[idx] == NULL) {
    slab[idx] = kmalloc(sizeof(slab_cache));
    slab[idx]->head_pd = NULL;
    slab[idx]->free_cnt = 0;
    slab[idx]->pd_cnt = 0;
    new_pd(idx);
    slab[idx]->cache_pd = slab_slab.head_pd;
  }
  if (slab[idx]->free_cnt > 0) {
    return pop_slab_cache(slab[idx]);
  }
  if (slab[idx]->remain < size) {
    new_pd(idx);
  }
  return slice_slab_remain(idx);
}

void free_page(void *ptr) {
  unsigned long pn = ptr_to_pn(ptr);
  log_hex("free page w/ pagenum", pn, LOG_DEBUG);
  unsigned long ord = get_page_ord(bs.page_stat[pn]);
  bs.page_stat[pn] = BUDDY_USE;

  while (ord < BUDDY_MAX_ORD - 1) {
    unsigned long bd = buddy_pn(pn, ord);
    if (get_page_flag(bs.page_stat[bd]) == BUDDY_FREE &&
        get_page_ord(bs.page_stat[bd]) == ord) {
      log_hex("coalesce order", ord, LOG_DEBUG);
      pop_cdl_list((cdl_list *)pn_to_ptr(bd));
      bs.page_stat[bd] = BUDDY_USE;
      ord++;
      pn = min(pn, bd);
    } else {
      break;
    }
  }
  set_page_flag(bs.page_stat[pn], BUDDY_FREE);
  set_page_ord(bs.page_stat[pn], ord);
  push_cdl_list(&(bs.free_list[ord]), (cdl_list *)pn_to_ptr(pn));
}

void free_reserve(void *ptr) {
  log_hex("free reserve", (unsigned long)ptr, LOG_DEBUG);
  void *st = ptr;
  void *ed = ptr;
  for (int i = 0; i < sa.reserve_cnt; i++) {
    if (sa.addr[i] == st) {
      ed = ed + sa.size[i];
    }
  }
  while (st != ed) {
    int st_ord = ptr_to_ord(st);
    int ed_ord = ptr_to_ord(ed);
    st_ord = min(st_ord, BUDDY_MAX_ORD - 1);
    ed_ord = min(ed_ord, BUDDY_MAX_ORD - 1);

    if (st_ord <= ed_ord) {
      free_page(st);
      st += (1 << (st_ord + PAGE_SIZE_CTZ));
    } else {
      ed -= (1 << (ed_ord + PAGE_SIZE_CTZ));
      free_page(ed);
    }
  }
}

void free_slab(void *ptr) {
  log_hex("free slab", (unsigned long)ptr, LOG_DEBUG);
  unsigned long ord = get_page_ord(bs.page_stat[ptr_to_pn(ptr)]);
  unsigned long pn = ptr_to_pn(ptr);
  page_descriptor *pd = (page_descriptor *)pn_to_ptr(((pn >> ord) << ord));
  push_l_list(&(pd->free_list), (l_list *)ptr);
  pd->free_cnt++;
  slab[pd->slab_idx]->free_cnt++;
  if (slab[pd->slab_idx]->cache_pd->free_list.next == NULL) {
    slab[pd->slab_idx]->cache_pd = pd;
  }
}

void *kmalloc(unsigned long size) {
  size = pad(size, MEM_PAD);
  void *chunk;
  disable_interrupt();
  if (size > SLAB_SIZE * MEM_PAD) {
    chunk = alloc_page(size);
  } else {
    chunk = alloc_slab(size);
  }
  enable_interrupt();
  return chunk;
}

void kfree(void *ptr) {
  disable_interrupt();
  int flag = get_page_flag(bs.page_stat[ptr_to_pn(ptr)]);
  if (flag == BUDDY_USE) {
    free_page(ptr);
  } else if (flag == RESRVE_USE) {
    free_reserve(ptr);
  } else if (flag == SLAB_USE) {
    free_slab(ptr);
  }
  enable_interrupt();
}

void init_kmalloc() {
  // init buddy system
  bs.page_stat = (unsigned char *)reserve_alloc(mem_size / PAGE_SIZE);
  log_hex("page stat at", (unsigned long)bs.page_stat, LOG_DEBUG);
  sort_reserve();
  for (int i = 0; i < BUDDY_MAX_ORD; i++) {
    init_cdl_list(&(bs.free_list[i]));
  }
  for (unsigned long i = 0; i < mem_size / PAGE_SIZE; i++) {
    bs.page_stat[i] = BUDDY_USE;
  }
  void *mem_itr = 0;
  for (int i = 0; i < sa.reserve_cnt; i++) {
    place_buddy(mem_itr, sa.addr[i], BUDDY_FREE);
    place_buddy(sa.addr[i], sa.addr[i] + sa.size[i], RESRVE_USE);
    mem_itr = sa.addr[i] + sa.size[i];
  }
  place_buddy(mem_itr, (void *)mem_size, BUDDY_FREE);

  // init slab
  for (int i = 0; i < SLAB_SIZE; i++) {
    slab[i] = NULL;
  }
  int slab_idx = size_to_idx(pad(sizeof(slab_cache), MEM_PAD));
  slab[slab_idx] = &slab_slab;
  slab_slab.head_pd = NULL;
  slab_slab.free_cnt = 0;
  slab_slab.pd_cnt = 0;
  new_pd(slab_idx);
  slab_slab.cache_pd = slab_slab.head_pd;
}
