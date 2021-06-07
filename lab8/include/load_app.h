# include "typedef.h"

# define USER_PRO_LR_START   0x80000
# define USER_PRO_SP_START   (0x100000000-0x10)   //4G-16B

int load_app(char* pathname, uint64_t ttbr0);
