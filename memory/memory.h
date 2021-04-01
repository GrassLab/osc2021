#define MEMORY_CNT ((int)256)
#define MEMORY_EXP ((int)4)
#define MEMORY_MAX ((int)8)
#define MEMORY_SIZE ((int)1 << MEMORY_EXP)

void memory_init();
void* memory_alloc(int size);
void memory_free(void* ptr);
