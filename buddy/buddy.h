#define BUDDY_CNT ((int)762)
#define BUDDY_EXP ((int)12)
#define BUDDY_MAX ((int)8)
#define BUDDY_BASE ((uint64_t)0x100000)
#define BUDDY_SIZE ((uint64_t)1 << BUDDY_EXP)

typedef struct {
  int index;
  int level;
} buddy_t;

void buddy_init();

buddy_t buddy_alloc(int level);

void buddy_free(buddy_t block);
