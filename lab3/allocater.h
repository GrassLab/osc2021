void init_header();
unsigned long int require_buddy_mem(int t);
void free_buddy_mem(unsigned long int t,int size);
void lookuppage();

unsigned long int my_malloc(int t);
void my_free(unsigned long int t);
void init_pool();
void lookuppool();