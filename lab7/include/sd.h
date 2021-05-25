# include "typedef.h"
# define SECTOR_SIZE       512
# define SANITY_CHECK      0xAA55

# define SECTOR_TABLE_COLS     16
# define SECTOR_TABLE_ROWS     (SECTOR_SIZE/SECTOR_TABLE_COLS)
# define SECTOR_TABLE_OFFSET   20

# define TO_HEX(n)       ((char)( (n<10) ? n+48 : n+55 ))

# define FAT_LOADED    1
# define FAT_DIRTY     2

struct partition{
  uint8_t boot_flag;
  uint8_t CHS_begin[3];
  uint8_t type_code;
  uint8_t CHS_end[3];
  uint32_t LBA_begin;
  uint32_t n_sector;
}__attribute__((packed));

struct MBR{
  char boot_code[446];
  struct partition partition[4];
  uint16_t sanity_check;
}__attribute__((packed));



struct fat32_first_sector{
  uint8_t   jump[3];
  uint8_t   oem[8];
  uint16_t  BPB_BytsPerSec;
  uint8_t   BPB_SecPerClus;
  uint16_t  BPB_RsvdSecCnt;
  uint8_t   BPB_NumFATs;
  uint8_t   dontcare1[19];
  uint32_t  BPB_FATSz32;
  uint16_t  mirror_flag;
  uint16_t  version;
  uint32_t  BPB_RootClus;
  uint8_t   dontcare[462];
  uint16_t  sanity_check;
}__attribute__((packed));

struct fat32_meta{
  uint32_t  LBA_begin;
  uint16_t  BPB_BytsPerSec;
  uint8_t   BPB_SecPerClus;
  uint16_t  BPB_RsvdSecCnt;
  uint8_t   BPB_NumFATs;
  uint32_t  BPB_FATSz32;
  uint32_t  BPB_RootClus;
  uint32_t  fat_begin_lba;
  uint32_t  cluster_begin_lba;
};

struct fat{
  int flag;
  uint32_t *data;
  char buf[SECTOR_SIZE];
};

void get_cluster(int n, char *buf);
uint32_t get_fat(uint32_t n);
void sdlistblock(int n);
void sdload();
