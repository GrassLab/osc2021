# include "sd.h"
# include "uart.h"
# include "sdhost.h"
# include "my_math.h"
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

uint32_t get_fat(uint32_t n){
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

void sdlistblock(int n){
  char buf[SECTOR_SIZE];
  readblock(n, buf);
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
  char buf[SECTOR_SIZE];
  char ct[20];
  readblock(0, buf);
  struct MBR *MBR = (struct MBR*) buf;
  int_to_str(MBR->sanity_check, ct);
  uart_puts((char *) &MBR->sanity_check );
  int_to_str(sizeof(struct fat32_first_sector), ct);
  uart_puts(ct);
  uart_puts("\n");
  uart_puts(ct);
  uart_puts("\n");
  if (MBR->sanity_check != SANITY_CHECK){
    log_puts("[Error] Sanity Check error. \n", WARNING);
    return ;
  }
  uint32_t first_partition_lba = MBR->partition[0].LBA_begin;
  sd_meta.LBA_begin = first_partition_lba;
  for (int i=0; i<4; i++){
    int_to_str(i+1, ct);
    log_puts("[Partition ", INFO);
    log_puts(ct, INFO);
    log_puts("]", INFO);
    if(MBR->partition[i].boot_flag == 0){
      log_puts(" : Empty\n", INFO);
      continue;
    }
    else{
      log_puts("\n\t[First sector] : ", INFO);
      int_to_str((int) MBR->partition[i].LBA_begin, ct);
      log_puts(ct, INFO);
      log_puts("\n\t[Number of sector] : ", INFO);
      int_to_str((int) MBR->partition[i].n_sector, ct);
      log_puts(ct, INFO);
      log_puts("\n", INFO);
    }
  }
  readblock(first_partition_lba, buf);
  struct fat32_first_sector *P_LBA = (struct fat32_first_sector*) buf;
  if (P_LBA->sanity_check != SANITY_CHECK){
    log_puts("[Error] Sanity Check error. \n", WARNING);
    return ;
  }
  sd_meta.BPB_BytsPerSec = P_LBA->BPB_BytsPerSec;
  sd_meta.BPB_SecPerClus = P_LBA->BPB_SecPerClus;
  sd_meta.BPB_RsvdSecCnt = P_LBA->BPB_RsvdSecCnt;
  sd_meta.BPB_NumFATs = P_LBA->BPB_NumFATs;
  sd_meta.BPB_FATSz32 = P_LBA->BPB_FATSz32;
  sd_meta.BPB_RootClus = P_LBA->BPB_RootClus;
  sd_meta.fat_begin_lba = first_partition_lba+P_LBA->BPB_RsvdSecCnt;
  sd_meta.cluster_begin_lba = sd_meta.fat_begin_lba+(P_LBA->BPB_NumFATs*P_LBA->BPB_FATSz32);
  show_sd_meta_info();
  fat_table_init();
}
