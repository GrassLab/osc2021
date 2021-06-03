# include "typedef.h"
# include "list.h"
# include "vfs.h"

# define SECTOR_SIZE       512
# define DIR_S_SIZE        32
# define SANITY_CHECK      0xAA55

# define SECTOR_TABLE_COLS     16
# define SECTOR_TABLE_ROWS     (SECTOR_SIZE/SECTOR_TABLE_COLS)
# define SECTOR_TABLE_OFFSET   20

# define TO_HEX(n)       ((char)( (n<10) ? n+48 : n+55 ))

# define FAT_LOADED    1
# define FAT_DIRTY     2

# define FAT_CLUS_DEL_ORD     0xE5
# define FAT_CLUS_ATTRIB_LFN  0x0F
# define FAT_CLUS_ATTRIB_DIR  0x10
# define FAT_CLUS_NAME_END    0x20

# define FAT_END_CLUSTER  0x0FFFFFF8

struct partition{
  uint8_t boot_flag;
  uint8_t CHS_begin[3];
  uint8_t type_code;
  uint8_t CHS_end[3];
  uint32_t LBA_begin;
  uint32_t n_sector;
};

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

struct cluster_data{
  uint32_t cluster_num;
  char buf[SECTOR_SIZE];
};

struct fat{
  int flag;
  uint32_t *data;
  char buf[SECTOR_SIZE];
};

struct fat32_file_info{
  char *name;
  uint32_t size;
  uint32_t first_cluster;
  enum dentry_type type;
  struct cluster_data cluster_data;
  int dir_t_no;
  list_head list;
};

struct SFN{
  char name[8];
  char extension_name[3];
  uint8_t attrib;
  uint8_t resereved;
  uint8_t create_time_s;
  uint16_t create_time;
  uint16_t create_date;
  uint16_t access_date;
  uint16_t first_cluster_h;
  uint16_t modify_time;
  uint16_t modify_date;
  uint16_t first_cluster_l;
  uint32_t size;
}__attribute__((packed));

struct LFN{
  uint8_t ord;
  char name1[10];
  uint8_t attrib;
  uint8_t resereved;
  uint8_t checksum;
  char name2[12];
  uint16_t first_cluster_l;
  char name3[4];
}__attribute__((packed));

union directory_t{
  struct SFN SFN;
  struct LFN LFN;
};

void get_cluster(int n, char *buf);
void put_cluster(int n, char *buf);
uint32_t get_fat(uint32_t n);
void sdlistblock(int n);
void sdload();
int get_fat_list_argc(uint32_t cluster);
union directory_t* get_dir_t(int n, struct cluster_data *argv);
void get_fat32_dir_list(uint32_t _cluster, list_head *r_list, struct cluster_data **fat_argv);
