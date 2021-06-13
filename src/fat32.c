# include "fat32.h"
# include "uart.h"
# include "sdhost.h"
# include "my_math.h"
# include "my_string.h"
# include "typedef.h"
# include "log.h"
# include "mem.h"

struct fat32_meta sd_meta;
struct fat *fat_table;

void get_fat_raw(int n, char *buf){
  int idx = sd_meta.fat_begin_lba+n;
  readblock(idx, buf);
}

void get_cluster(int n, char *buf){
  int idx = (sd_meta.cluster_begin_lba+(n-sd_meta.BPB_RootClus)*sd_meta.BPB_SecPerClus);
  readblock(idx, buf);
}

void put_cluster(int n, char *buf){
  int idx = (sd_meta.cluster_begin_lba+(n-sd_meta.BPB_RootClus)*sd_meta.BPB_SecPerClus);
  writeblock(idx, buf);
}

uint32_t get_fat(uint32_t n){
  if (n == FAT_END_CLUSTER) return FAT_END_CLUSTER;
  int hidx = n/(SECTOR_SIZE/sizeof(uint32_t));
  int lidx = n%(SECTOR_SIZE/sizeof(uint32_t));
  if ((fat_table[hidx].flag & FAT_LOADED) == 0){
    get_fat_raw(hidx, fat_table[hidx].buf);
  }
  return fat_table[hidx].data[lidx];
}

void fat_table_init(){
  fat_table = MALLOC(struct fat, sd_meta.BPB_FATSz32);
  for (uint32_t i=0; i<sd_meta.BPB_FATSz32; i++){
    fat_table[i].flag = 0;
    fat_table[i].data = (uint32_t*) fat_table[i].buf;
  }
}

void showblock(int n, char *buf){
  char data[SECTOR_TABLE_OFFSET+SECTOR_TABLE_COLS*4+3];
  for (int i=0; i<SECTOR_TABLE_OFFSET+1; i++){
    data[i] = ' ';
  }
  for (int c=0; c<SECTOR_TABLE_COLS; c++){
    int msb = c/16;
    int lsb = c%16;
    data[c*4+SECTOR_TABLE_OFFSET+1] = (c%8 == 0) ? '|' : ' ';
    data[c*4+SECTOR_TABLE_OFFSET+2] = TO_HEX(msb);
    data[c*4+SECTOR_TABLE_OFFSET+3] = TO_HEX(lsb);
    data[c*4+SECTOR_TABLE_OFFSET+4] = ' ';
  }
  data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+1] = '|';
  data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+2] = '\0';
  uart_puts(data);
  uart_puts("\n");
  for (int i=0; i<SECTOR_TABLE_OFFSET+SECTOR_TABLE_COLS*4+2; i++){
    data[i] = '=';
  }
  data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+2] = '\0';
  uart_puts(data);
  uart_puts("\n");
  data[SECTOR_TABLE_OFFSET] = ' ';
  for (int r=0; r<SECTOR_TABLE_ROWS; r++){
    int_to_hex_align(n*SECTOR_SIZE+r*SECTOR_TABLE_COLS, data, SECTOR_TABLE_OFFSET-1);
    for (int c=0; c<SECTOR_TABLE_COLS; c++){
      int msb = ((int)buf[r*SECTOR_TABLE_COLS+c])/16;
      int lsb = ((int)buf[r*SECTOR_TABLE_COLS+c])%16;
      data[c*4+SECTOR_TABLE_OFFSET+1] = (c%8 == 0) ? '|' : ' ';
      data[c*4+SECTOR_TABLE_OFFSET+2] = TO_HEX(msb);
      data[c*4+SECTOR_TABLE_OFFSET+3] = TO_HEX(lsb);
      data[c*4+SECTOR_TABLE_OFFSET+4] = ' ';
    }
    data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+1] = '|';
    data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+2] = '\0';
    uart_puts(data);
    uart_puts("\n");
  }
  for (int i=0; i<SECTOR_TABLE_OFFSET+SECTOR_TABLE_COLS*4+2; i++){
    data[i] = '=';
  }
  data[SECTOR_TABLE_COLS*4+SECTOR_TABLE_OFFSET+2] = '\0';
  uart_puts(data);
  uart_puts("\n");
}

void sdlistblock(int n){
  char buf[SECTOR_SIZE];
  readblock(n, buf);
  showblock(n, buf);
}

void show_sd_meta_info(){
  char ct[20];
  int_to_str(sd_meta.BPB_BytsPerSec, ct);
  log_puts("\n[Partition 1]\n", INFO);
  log_puts("\t[BytsPerSec] : ", INFO);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  int_to_str(sd_meta.BPB_SecPerClus, ct);
  log_puts("\t[SecPerClus] : ", INFO);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  int_to_str(sd_meta.BPB_RsvdSecCnt, ct);
  log_puts("\t[RsvdSecCnt] : ", INFO);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  int_to_str(sd_meta.BPB_NumFATs, ct);
  log_puts("\t[NumFATs] : ", INFO);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  int_to_str(sd_meta.BPB_FATSz32, ct);
  log_puts("\t[FATSz32] : ", INFO);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  int_to_str(sd_meta.BPB_RootClus, ct);
  log_puts("\t[RootClus] : ", INFO);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  int_to_str(sd_meta.fat_begin_lba, ct);
  log_puts("\t[FAT begin sector] : ", INFO);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
  int_to_str(sd_meta.cluster_begin_lba, ct);
  log_puts("\t[Cluster begin sector] : ", INFO);
  log_puts(ct, INFO);
  log_puts("\n", INFO);
}

void sdload(){
  log_puts("[INFO] Loading SD card.\n", INFO);
  char buf[SECTOR_SIZE];
  char ct[20];
  readblock(0, buf);
  showblock(0, buf);
  struct MBR *MBR = (struct MBR*) buf;

  if (MBR->sanity_check != SANITY_CHECK){
    log_puts("[Error] Sanity Check error. \n", WARNING);
    return ;
  }
  struct partition p;
  memcpy(buf+446, &p, sizeof(struct partition));
  uint32_t first_partition_lba = p.LBA_begin;
  sd_meta.LBA_begin = first_partition_lba;
  for (int i=0; i<4; i++){
    struct partition pt;
    memcpy(buf+446+sizeof(struct partition)*i, &pt, sizeof(struct partition));
    int_to_str(i+1, ct);
    log_puts("[Partition ", INFO);
    log_puts(ct, INFO);
    log_puts("]", INFO);
    if(pt.type_code == EMPTY_PARTITION_TYPE){
      log_puts(" : Empty\n", INFO);
      continue;
    }
    else if (pt.type_code == FAT32_PARTITION_TYPE1){
      log_puts(" : FAT32 with CHS addressing", INFO);
      log_puts("\n\t[First sector] : ", INFO);
      int_to_str((int) pt.LBA_begin, ct);
      log_puts(ct, INFO);
      log_puts("\n\t[Number of sector] : ", INFO);
      int_to_str((int) pt.n_sector, ct);
      log_puts(ct, INFO);
      log_puts("\n", INFO);
    }
    else if (pt.type_code == FAT32_PARTITION_TYPE2){
      log_puts(" : FAT32 with LBA", INFO);
      log_puts("\n\t[First sector] : ", INFO);
      int_to_str((int) pt.LBA_begin, ct);
      log_puts(ct, INFO);
      log_puts("\n\t[Number of sector] : ", INFO);
      int_to_str((int) pt.n_sector, ct);
      log_puts(ct, INFO);
      log_puts("\n", INFO);
    }
  }
  readblock(first_partition_lba, buf);
  struct fat32_first_sector P_LBA;
  memcpy(buf, &P_LBA, sizeof(struct fat32_first_sector));
  if (P_LBA.sanity_check != SANITY_CHECK){
    log_puts("[Error] Sanity Check error. \n", WARNING);
    return ;
  }
  
  sd_meta.BPB_BytsPerSec = 512;
  sd_meta.BPB_SecPerClus = P_LBA.BPB_SecPerClus;
  sd_meta.BPB_RsvdSecCnt = P_LBA.BPB_RsvdSecCnt;
  sd_meta.BPB_NumFATs = P_LBA.BPB_NumFATs;
  sd_meta.BPB_FATSz32 = P_LBA.BPB_FATSz32;
  sd_meta.BPB_RootClus = P_LBA.BPB_RootClus;
  sd_meta.fat_begin_lba = first_partition_lba+P_LBA.BPB_RsvdSecCnt;
  sd_meta.cluster_begin_lba = sd_meta.fat_begin_lba+(P_LBA.BPB_NumFATs*P_LBA.BPB_FATSz32);
  show_sd_meta_info();
  log_puts("\t[All sector data]\n", INFO);
  showblock(first_partition_lba, buf);
  
  fat_table_init();
  list_head list;
  list_head_init(&list);
  log_puts("[INFO] Loading SD card Done.\n", INFO);
}

int get_fat_list_argc(uint32_t cluster){
  int r = 0;
  while(cluster < FAT_END_CLUSTER){
    r++;
    cluster = get_fat(cluster);
  }
  return r;
}

void get_fat_list_argv(uint32_t cluster, struct cluster_data *argv){
  int r = 0;
  while(cluster < FAT_END_CLUSTER){
    argv[r].cluster_num = cluster;
    get_cluster(cluster, argv[r].buf);
    r++;
    cluster = get_fat(cluster);
  }
  argv[r].cluster_num = cluster;
}

union directory_t* get_dir_t(int n, struct cluster_data *argv){
  if (argv[n/(SECTOR_SIZE/DIR_S_SIZE)].cluster_num >= FAT_END_CLUSTER) return 0;
  union directory_t *dt = (union directory_t*)(argv[n/(SECTOR_SIZE/DIR_S_SIZE)].buf);
  union directory_t *r = &dt[n%(SECTOR_SIZE/DIR_S_SIZE)];
  if(r->LFN.ord != 0){
    return r;
  }
  return 0;
}

static void get_LFN_word(union directory_t *data, uint16_t *list){
  memcpy(data->LFN.name1, list, 10);
  memcpy(data->LFN.name2, list+5, 12);
  memcpy(data->LFN.name3, list+11, 4);
}

void get_fat32_dir_list(uint32_t _cluster, list_head *r_list, struct cluster_data **fat_argv){
  int fat_argc = get_fat_list_argc(_cluster);
  log_puts("debug2\n", FINE);
  *fat_argv = MALLOC(struct cluster_data, fat_argc+1);
  log_puts("debug3\n", FINE);
  get_fat_list_argv(_cluster, *fat_argv);
  log_puts("debug4\n", FINE);
  int idx = 0;
  list_head_init(r_list);
  while(1){
    union directory_t *data = get_dir_t(idx, *fat_argv);
    if (data == 0){
      break;
    }
    if (data->LFN.ord == FAT_CLUS_DEL_ORD){
      log_puts("Find del dir_t\n", FINE);
      idx++;
      continue;
    }
    if (data->LFN.attrib == FAT_CLUS_ATTRIB_LFN){
      uint8_t ord = data->LFN.ord;
      ord &= 0xF;
      idx+=ord;
      char *name = MALLOC(char, ord*13*3+1);
      int name_p = 0;
      for (int i = 1; i<=ord; i++){
        union directory_t *dt = get_dir_t(idx-i, *fat_argv);
        uint16_t LFN_words[13];
        get_LFN_word(dt, LFN_words);
        for (int j=0; j<13; j++){
          uint16_t ucs = LFN_words[j];
          if (ucs <= 0xFF){
            name[name_p] = (uint8_t) ucs;
            name_p++;
          }
          else{
            int tt = to_utf8(ucs, name+name_p);
            name_p+=tt;
          }
          if (ucs == 0) break;
        }
      }
      if (name[name_p-1] != '\0') name[name_p] = '\0';
      
      data = get_dir_t(idx, *fat_argv);
      struct fat32_file_info *t = MALLOC(struct fat32_file_info, 1);
      list_add_next(&(t->list), r_list);
      t->first_cluster = ( (data->SFN.first_cluster_h) << 16) | (data->SFN.first_cluster_l);
      t->size = data->SFN.size;
      t->type = ((data->SFN.attrib & FAT_CLUS_ATTRIB_DIR) >0) ? DIR : FILE;
      t->name = name;
      t->dir_t_no = idx;
      
      log_puts("Get one LFN ", FINE);
      if (t->type == DIR){
        log_puts("DIR : ", FINE);
      }
      else{
        log_puts("FILE: ", FINE);
      }
      log_puts(name, FINE);
      log_puts("\n", FINE);
    }
    else{
      log_puts("Get one SFN ", FINE);
      struct fat32_file_info *t = MALLOC(struct fat32_file_info, 1);
      list_add_next(&(t->list), r_list);
      t->first_cluster = ( (data->SFN.first_cluster_h) << 16) | (data->SFN.first_cluster_l);
      t->size = data->SFN.size;
      t->type = ((data->SFN.attrib & FAT_CLUS_ATTRIB_DIR) >0) ? DIR : FILE;
      t->dir_t_no = idx;
      char *name = MALLOC(char, 14);
      t->name = name;
      int name_idx = 0;
      for (name_idx=0; name_idx<8; name_idx++){
        if (data->SFN.name[name_idx] == FAT_CLUS_NAME_END){
          name[name_idx] = '\0';
          break;
        }
        name[name_idx] = data->SFN.name[name_idx];
      }
      name[name_idx] = (data->SFN.extension_name[0] == FAT_CLUS_NAME_END) ? '\0' : '.';
      name_idx++;
      for (int i=0; i<3; i++){
        if (data->SFN.extension_name[i] == FAT_CLUS_NAME_END){
          name[name_idx] = '\0';
          break;
        }
        name[name_idx] = data->SFN.extension_name[i];
        name_idx++;
      }
      name[name_idx] = '\0';
      if (t->type == DIR){
        log_puts("DIR : ", FINE);
      }
      else{
        log_puts("FILE: ", FINE);
      }
      log_puts(name, FINE);
      log_puts("\n", FINE);
    }
    idx++;
  }
}
