#include "mem.h"

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

#define BUDDY_FREE 0x0
#define BUDDY_USE 0x20
#define SLAB_USE 0x40
#define RESRVE_USE 0x60

typedef struct buddy_system {
  cdl_list free_list[BUDDY_MAX_ORD];
} buddy_system;

// typedef struct startup_allocator {
//   void *addr[STARTUP_MAX];
//   unsigned long long size[STARTUP_MAX];
// } startup_allocator;

// typedef struct cache_list {
//   struct cache_list *next;
// } cache_list;

// typedef struct page_descriptor {
//   void *page;
//   struct page_descriptor *next_pd;
//   cache_list *free_list;
//   unsigned int free_count;
// } page_descriptor;

// static inline void *pop_cache(cache_list **cl) {
//   void *addr = (void *)*cl;
//   *cl = (*cl)->next;
//   return addr;
// }

// static inline void push_cache(cache_list **cl, cache_list *new_chunk) {
//   new_chunk->next = (*cl);
//   (*cl) = new_chunk;
// }

// typedef struct slab_cache {
//   struct slab_cache *next_slab;
//   page_descriptor *head_pd;
//   page_descriptor *cache_pd;
//   void *page_slice_pos;
//   unsigned int size;
//   unsigned int free_count;
//   unsigned int page_remain;
// } slab_cache;

// void *pop_slab_cache(slab_cache *sc) {
//   if (sc->cache_pd->free_list == NULL) {
//     page_descriptor *pd_itr = sc->head_pd;
//     while (pd_itr->free_list == NULL) {
//       pd_itr = pd_itr->next_pd;
//     }
//     sc->cache_pd = pd_itr;
//   }
//   page_descriptor *pd = sc->cache_pd;
//   void *new_chunk = pop_cache(&(pd->free_list));
//   pd->free_count--;
//   sc->free_count--;
//   log("pop slab cache\n");
//   return new_chunk;
// }

// slab_cache *sc_slab_tok;
// slab_cache *pd_slab_tok;

// unsigned long long mem_size = 0x40000000;  // 1 GB
// unsigned long long reserve_count = 0;
// buddy_system bs;
// startup_allocator sa;
// char *buddy_stat;
// slab_cache *slab_st;

// void *kmalloc(unsigned long size) {
//   size = pad(size, 16);
//   if (size > PAGE_SIZE / 2) {
//     return alloc_page(size);
//   } else {
//     return alloc_slab(register_slab(size));
//   }
// }

// void kfree(void *ptr) {
//   int flag = get_buddy_flag(buddy_stat[ptr_to_pagenum(ptr)]);
//   if (flag == BUDDY_USE) {
//     free_page(ptr);
//   } else if (flag == RESRVE_USE) {
//     free_reserve(ptr);
//   } else if (flag == SLAB_USE) {
//     free_unknow_slab(ptr);
//   }
// }

// void sort_reserve() {
//   for (int i = reserve_count - 1; i >= 0; i--) {
//     for (int j = 0; j < i; j++) {
//       if ((unsigned long long)sa.addr[j] > (unsigned long long)sa.addr[j +
//       1]) {
//         void *tmp_addr = sa.addr[j];
//         sa.addr[j] = sa.addr[j + 1];
//         sa.addr[j + 1] = tmp_addr;
//         unsigned long long tmp_size = sa.size[j];
//         sa.size[j] = sa.size[j + 1];
//         sa.size[j + 1] = tmp_size;
//       }
//     }
//   }
// }

// int check_reserve_collision(void *a1, size_t s1, void *a2, size_t s2) {
//   void *e1 = (void *)((unsigned long long)a1 + s1);
//   void *e2 = (void *)((unsigned long long)a2 + s2);
//   return ((a2 >= a1) && (a2 < e1)) || ((e2 > a1) && (e2 <= e1)) ||
//          ((a1 >= a2) && (a1 < e2)) || ((e1 > a2) && (e1 <= e2));
// }

// int reserve_mem(void *addr, unsigned long size) {
//   if (((unsigned long long)addr & 0xfff) != 0 || (size & 0xfff) != 0) {
//     log("reserve mem require page align\n");
//     return -1;
//   }
//   if (reserve_count >= STARTUP_MAX) {
//     log("no reserve slot available\n");
//     return -1;
//   } else {
//     for (int i = 0; i < reserve_count; i++) {
//       if (check_reserve_collision(sa.addr[i], sa.size[i], addr, size)) {
//         log("reserve collision\n");
//         return -1;
//       }
//     }
//     sa.addr[reserve_count] = addr;
//     sa.size[reserve_count] = size;
//     reserve_count++;
//     return 0;
//   }
// }

// void _place_buddy(void *ptr, int ord, int flag) {
//   unsigned long long idx = (unsigned long long)ptr >> PAGE_SIZE_CTZ;

//   set_buddy_ord(buddy_stat[idx], ord);
//   set_buddy_flag(buddy_stat[idx], flag);

//   if (flag == BUDDY_FREE) {
//     push_list(&bs.free_list[ord], (list_head *)ptr);
//   }
// }

// void place_buddy(void *st, void *ed, int flag) {
//   while (st != ed) {
//     int st_ord = get_order(st);
//     int ed_ord = get_order(ed);
//     if (st_ord >= BUDDY_MAX) {
//       st_ord = BUDDY_MAX - 1;
//     }
//     if (ed_ord >= BUDDY_MAX) {
//       ed_ord = BUDDY_MAX - 1;
//     }
//     if (st_ord <= ed_ord) {
//       _place_buddy(st, st_ord, flag);
//       st += (1 << (st_ord + PAGE_SIZE_CTZ));
//     } else {
//       ed -= (1 << (ed_ord + PAGE_SIZE_CTZ));
//       _place_buddy(ed, ed_ord, flag);
//     }
//   }
// }

// void init_buddy(char *stat_ptr) {
//   buddy_stat = stat_ptr;
//   sort_reserve();
//   for (int i = 0; i < BUDDY_MAX; i++) {
//     init_list(&(bs.free_list[i]));
//   }
//   for (unsigned long i = 0; i < mem_size / PAGE_SIZE; i++) {
//     buddy_stat[i] = INIT_PAGE;
//   }
//   void *mem_itr = 0;
//   for (int i = 0; i < reserve_count; i++) {
//     place_buddy(mem_itr, sa.addr[i], BUDDY_FREE);
//     place_buddy(sa.addr[i], sa.addr[i] + sa.size[i], RESRVE_USE);
//     mem_itr = sa.addr[i] + sa.size[i];
//   }
//   place_buddy(mem_itr, (void *)mem_size, BUDDY_FREE);
// }

// void *alloc_page(size_t size) {
//   log("alloc page: ");
//   log_hex(size);
//   log("\n");
//   size = pad(size, PAGE_SIZE);
//   size_t target_ord = 51 - __builtin_clzl(size);
//   if ((((1 << (target_ord + 12)) - 1) & size) != 0) {
//     target_ord++;
//   }

//   size_t find_ord = target_ord;
//   while (list_empty(&bs.free_list[find_ord])) {
//     find_ord++;
//     if (find_ord >= BUDDY_MAX) {
//       log("out of memory\n");
//       return NULL;
//     }
//   }
//   void *new_chunk = (void *)bs.free_list[find_ord].fd;
//   pop_list((list_head *)new_chunk);
//   size_t pn = ptr_to_pagenum(new_chunk);
//   set_buddy_flag(buddy_stat[pn], BUDDY_USE);
//   set_buddy_ord(buddy_stat[pn], target_ord);

//   while (find_ord > target_ord) {
//     log("release\n");
//     find_ord--;
//     size_t bd = buddy_pagenum(pn, find_ord);
//     set_buddy_flag(buddy_stat[bd], BUDDY_FREE);
//     set_buddy_ord(buddy_stat[bd], find_ord);
//     push_list(&bs.free_list[find_ord], (list_head *)pagenum_to_ptr(bd));
//   }

//   return new_chunk;
// }

// void free_page(void *ptr) {
//   log("free page: ");
//   log_hex((unsigned long long)ptr);
//   log("\n");
//   unsigned long pagenum = ptr_to_pagenum(ptr);
//   unsigned long ord = get_buddy_ord(buddy_stat[pagenum]);
//   buddy_stat[pagenum] = INIT_PAGE;

//   while (ord < BUDDY_MAX - 1) {
//     unsigned long buddy = buddy_pagenum(pagenum, ord);
//     if (get_buddy_flag(buddy_stat[buddy]) == BUDDY_FREE &&
//         get_buddy_ord(buddy_stat[buddy]) == ord) {
//       log("coalesce\n");
//       pop_list((list_head *)pagenum_to_ptr(buddy));
//       buddy_stat[buddy] = INIT_PAGE;
//       ord++;
//       pagenum = pagenum < buddy ? pagenum : buddy;
//     } else {
//       break;
//     }
//   }
//   set_buddy_flag(buddy_stat[pagenum], BUDDY_FREE);
//   set_buddy_ord(buddy_stat[pagenum], ord);
//   push_list(&bs.free_list[ord], pagenum_to_ptr(pagenum));
// }

// void free_reserve(void *ptr) {
//   log("free reserve: ");
//   log_hex((unsigned long long)ptr);
//   log("\n");
//   void *st = ptr;
//   void *ed;
//   for (int i = 0; i < reserve_count; i++) {
//     if (sa.addr[i] == st) {
//       ed = st + sa.size[i];
//     }
//   }

//   while (st != ed) {
//     int st_ord = get_order(st);
//     int ed_ord = get_order(ed);
//     if (st_ord >= BUDDY_MAX) {
//       st_ord = BUDDY_MAX - 1;
//     }
//     if (ed_ord >= BUDDY_MAX) {
//       ed_ord = BUDDY_MAX - 1;
//     }
//     if (st_ord <= ed_ord) {
//       free_page(st);
//       st += (1 << (st_ord + PAGE_SIZE_CTZ));
//     } else {
//       ed -= (1 << (ed_ord + PAGE_SIZE_CTZ));
//       free_page(ed);
//     }
//   }
// }

// void log_buddy() {
//   for (int i = 0; i < BUDDY_MAX; i++) {
//     list_head *l = &bs.free_list[i];
//     list_head *head = l;
//     int count = 0;
//     log("ord: ");
//     log_hex(i);
//     log("\n");
//     while (l->fd != head) {
//       log("block: ");
//       log_hex((unsigned long long)l->fd);
//       log("\n");
//       l = l->fd;
//       count++;
//     }
//     log("total: ");
//     log_hex(count);
//     log("\n");
//   }
// }

// void check_buddy_stat() {
//   for (unsigned long i = 0; i < mem_size / PAGE_SIZE; i++) {
//     if (buddy_stat[i] != INIT_PAGE) {
//       if (get_buddy_flag(buddy_stat[i]) == BUDDY_FREE) {
//         log("buddy free: ");
//       } else if (get_buddy_flag(buddy_stat[i]) == RESRVE_USE) {
//         log("reserve use: ");
//       } else if (get_buddy_flag(buddy_stat[i]) == BUDDY_USE) {
//         log("buddy use: ");
//       } else {
//         log("slab use :");
//       }
//       log_hex(i);
//       log("\n");
//     }
//   }
// }

// void init_slab() {
//   // slab_cache for slab_cache type
//   slab_cache *sc_slab = (slab_cache *)alloc_page(PAGE_SIZE);
//   // slab_cache for page_descriptor type
//   slab_cache *pd_slab = sc_slab + 1;
//   // page_descriptor for slab_cache type
//   page_descriptor *sc_page = (page_descriptor *)alloc_page(PAGE_SIZE);
//   // page_descriptor for page_descriptor type
//   page_descriptor *pd_page = sc_page + 1;

//   set_buddy_flag(buddy_stat[ptr_to_pagenum((void *)sc_slab)], SLAB_USE);
//   set_buddy_flag(buddy_stat[ptr_to_pagenum((void *)sc_page)], SLAB_USE);

//   sc_slab->next_slab = pd_slab;
//   sc_slab->free_count = 0;
//   sc_slab->cache_pd = sc_page;
//   sc_slab->page_remain = PAGE_SIZE - pad(sizeof(slab_cache), 16) * 2;
//   sc_slab->page_slice_pos = pd_slab + 1;
//   sc_slab->head_pd = sc_page;
//   sc_slab->size = pad(sizeof(slab_cache), 16);

//   pd_slab->next_slab = NULL;
//   pd_slab->free_count = 0;
//   pd_slab->cache_pd = pd_page;
//   pd_slab->page_remain = PAGE_SIZE - pad(sizeof(page_descriptor), 16) * 2;
//   pd_slab->page_slice_pos = pd_page + 1;
//   pd_slab->head_pd = pd_page;
//   pd_slab->size = pad(sizeof(page_descriptor), 16);

//   sc_page->free_list = NULL;
//   sc_page->next_pd = NULL;
//   sc_page->page = (void *)sc_slab;
//   sc_page->free_count = 0;

//   pd_page->free_list = NULL;
//   pd_page->next_pd = NULL;
//   pd_page->page = (void *)sc_page;
//   pd_page->free_count = 0;

//   slab_st = sc_slab;
//   sc_slab_tok = sc_slab;
//   pd_slab_tok = pd_slab;
// }

// void pd_self_alloc() {
//   page_descriptor *new_pd = (page_descriptor *)alloc_page(PAGE_SIZE);
//   set_buddy_flag(buddy_stat[ptr_to_pagenum((void *)new_pd)], SLAB_USE);
//   new_pd->page = (void *)new_pd;
//   new_pd->free_count = 0;
//   new_pd->free_list = NULL;
//   new_pd->next_pd = pd_slab_tok->head_pd;
//   pd_slab_tok->head_pd = new_pd;
//   pd_slab_tok->page_remain = PAGE_SIZE - pad(sizeof(page_descriptor), 16);
//   pd_slab_tok->page_slice_pos = new_pd + 1;
//   log("pd self allocate\n");
// }

// page_descriptor *new_pd() {
//   page_descriptor *pd;
//   if (pd_slab_tok->free_count > 0) {
//     pd = (page_descriptor *)pop_slab_cache(pd_slab_tok);
//   } else {
//     if (pd_slab_tok->page_remain < pad(sizeof(page_descriptor), 16)) {
//       pd_self_alloc();
//     }

//     pd_slab_tok->page_remain -= pad(sizeof(page_descriptor), 16);
//     pd = (page_descriptor *)(pd_slab_tok->page_slice_pos);
//     pd_slab_tok->page_slice_pos += pad(sizeof(page_descriptor), 16);
//   }
//   pd->free_count = 0;
//   pd->free_list = NULL;
//   pd->next_pd = NULL;
//   pd->page = alloc_page(PAGE_SIZE);
//   set_buddy_flag(buddy_stat[ptr_to_pagenum(pd->page)], SLAB_USE);
//   log("new pd at: ");
//   log_hex((unsigned long long)pd->page);
//   log("\n");
//   return pd;
// }

// void *register_slab(size_t size) {
//   slab_cache *sc = slab_st;
//   while (sc != NULL) {
//     if (sc->size == size) {
//       log("find token\n");
//       return (void *)sc;
//     }
//     sc = sc->next_slab;
//   }
//   sc = (slab_cache *)alloc_slab((void *)sc_slab_tok);
//   sc->head_pd = new_pd();
//   sc->cache_pd = sc->head_pd;
//   sc->page_slice_pos = sc->head_pd->page;
//   sc->size = size;
//   sc->free_count = 0;
//   sc->page_remain = PAGE_SIZE;
//   sc->next_slab = slab_st;
//   slab_st = sc;
//   log("new slab\n");
//   return (void *)sc;
// }

// static inline void *slice_remain_slab(slab_cache *sc) {
//   sc->page_remain -= sc->size;
//   void *addr = sc->page_slice_pos;
//   sc->page_slice_pos += sc->size;
//   log("slice remain: ");
//   log_hex(sc->page_remain);
//   log("\n");
//   return addr;
// }

// void *alloc_slab(void *slab_tok) {
//   slab_cache *sc = (slab_cache *)slab_tok;
//   if (sc->free_count > 0) {
//     return pop_slab_cache(sc);
//   }
//   if (sc->page_remain < sc->size) {
//     if (sc == pd_slab_tok) {
//       pd_self_alloc();
//     } else {
//       page_descriptor *pd = new_pd();
//       pd->next_pd = sc->head_pd;
//       sc->head_pd = pd;
//       sc->page_slice_pos = sc->head_pd->page;
//       sc->page_remain = PAGE_SIZE;
//     }
//   }
//   return slice_remain_slab(sc);
// }

// void _free_slab(void *ptr, slab_cache *sc, page_descriptor *pd) {
//   log("free slab inter\n");
//   push_cache(&(pd->free_list), (cache_list *)ptr);
//   pd->free_count++;
//   sc->free_count++;
//   if (sc->cache_pd->free_list == NULL) {
//     sc->cache_pd = pd;
//   }
// }

// void free_unknow_slab(void *ptr) {
//   log("free slab unknow\n");
//   slab_cache *sc = slab_st;
//   while (sc != NULL) {
//     page_descriptor *pd = sc->head_pd;
//     while (pd != NULL) {
//       if (ptr_to_pagenum(pd->page) == ptr_to_pagenum(ptr)) {
//         _free_slab(ptr, sc, pd);
//         return;
//       }
//       pd = pd->next_pd;
//     }
//     sc = sc->next_slab;
//   }
// }

// void free_slab(void *ptr, void *slab) {
//   log("free slab\n");
//   page_descriptor *pd = ((slab_cache *)slab)->head_pd;
//   while (pd != NULL) {
//     if (ptr_to_pagenum(pd->page) == ptr_to_pagenum(ptr)) {
//       _free_slab(ptr, ((slab_cache *)slab), pd);
//       return;
//     }
//     pd = pd->next_pd;
//   }
// }

// void check_slab() {
//   slab_cache *sc = slab_st;
//   while (sc != NULL) {
//    log("slab cache\n");
//    log("size: ");
//    log_hex(sc->size);
//    log("\n");
//    sc = sc->next_slab;
//   }
// }
