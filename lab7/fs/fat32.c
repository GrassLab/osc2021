#include "fat32.h"
#include <printf.h>
#include <uart.h>
#include <string.h>
#include <varied.h>

void fat32_init() {
  parse_mbr();

  for(int i = 0; i < 4; i++) {
    if(_mbr.partitions[i].lba != 0) {
      
      _boot_sectors[i] = parse_boot_sector(_mbr.partitions[i].lba);
      
      if(_boot_sectors[i] != null)
        parse_root_directory(_mbr.partitions[i].lba, _boot_sectors[i]);
    }
  }
}

void parse_mbr() {
  char buf[BLOCK_SIZE];
  
  readblock(0, buf);
  //copy mbr
  memcpy((char *)&_mbr, buf, sizeof(_mbr));

  strncpy(buf, (char *)_mbr.signature, 2);
  buf[3] = '\0';

  printf("mbr: \n\
  addr: %x\n\
  partition 1: type code: %x, lba: %x: num_of_sectors: %x\n\
  partition 2: type code: %x, lba: %x: num_of_sectors: %x\n\
  partition 3: type code: %x, lba: %x: num_of_sectors: %x\n\
  partition 4: type code: %x, lba: %x: num_of_sectors: %x\n\
  signature1: %x\n\
  signature2: %x\n",
  &_mbr,
  _mbr.partitions[0].type_code, _mbr.partitions[0].lba, _mbr.partitions[0].num_of_sectors,
  _mbr.partitions[1].type_code, _mbr.partitions[1].lba, _mbr.partitions[1].num_of_sectors,
  _mbr.partitions[2].type_code, _mbr.partitions[2].lba, _mbr.partitions[2].num_of_sectors,
  _mbr.partitions[3].type_code, _mbr.partitions[3].lba, _mbr.partitions[3].num_of_sectors,
  buf[0],
  buf[1]);
}

void* parse_boot_sector(uint32_t lba) {
  char buf[BLOCK_SIZE];
  
  struct boot_sector *boot_sector;

  readblock(lba, buf);
  
  boot_sector = (struct boot_sector*)varied_malloc(sizeof(struct boot_sector));

  if(boot_sector == null) 
    return null;
  
  //printf("size: %d\n", sizeof(struct boot_sector));
  memcpy((char*)boot_sector, buf, sizeof(struct boot_sector));
  
  //boot_sector = buf;
  printf("boot_sector: \n\
  addr: %x\n\
  char oem_name[8]: %s\n\
  uint16_t bytes_per_sector: %d\n\
  uint8_t sectors_per_cluster: %d\n\
  uint16_t num_of_reserved_sectors: %d\n\
  uint8_t num_of_fats: %d\n\
  uint16_t max_root_entries: %d\n\
  uint16_t num_of_sectors: %d\n\
  uint8_t media_descriptor: %d\n\
  uint16_t sectors_per_fat: %d\n\
  uint16_t sectors_per_track: %d\n\
  uint16_t num_of_head: %d\n\
  uint32_t num_of_hidden_sectors: %d\n\
  uint32_t num_of_sectors_large: %d\n\
  uint32_t sectors_per_fat_large_fat32: %d\n\
  uint16_t flag: %x\n\
  uint16_t version: %d\n\
  uint32_t cluster_num_of_root_dir: %d\n\ 
  uint16_t num_of_fs_info_sectors: %d\n\
  uint16_t num_of_backup_sectors: %d\n\
  uint8_t drvie_num: %d\n",
  boot_sector,
  boot_sector->oem_name,
  boot_sector->bytes_per_sector,
  boot_sector->sectors_per_cluster,
  boot_sector->num_of_reserved_sectors,
  boot_sector->num_of_fats,
  boot_sector->max_root_entries,
  boot_sector->num_of_sectors,
  boot_sector->media_descriptor,
  boot_sector->sectors_per_fat,
  boot_sector->sectors_per_track,
  boot_sector->num_of_head,
  boot_sector->num_of_hidden_sectors,
  boot_sector->num_of_sectors_large,
  boot_sector->sectors_per_fat_large_fat32,
  boot_sector->flag,
  boot_sector->version,
  boot_sector->cluster_num_of_root_dir,
  boot_sector->sector_num_of_fs_info_sector,
  boot_sector->sector_num_of_backup_sector,
  boot_sector->drvie_num);

  return boot_sector;
}

void parse_root_directory(uint32_t lba, struct boot_sector* _boot_sector) {
  char buf[BLOCK_SIZE];
  struct directory_entry* d_entry;
  char filename[D_ENTRY_NAME_SIZE + D_ENTRY_EXTENSION_SIZE + 1];
  int i;
  //root directory
  readblock(lba + _boot_sector->num_of_reserved_sectors + _boot_sector->sectors_per_fat_large_fat32 * 2, buf);
  
  i = 0;
  while(1) {
    d_entry = buf + i;
    
    if(*((char *)d_entry) == '\x00') {
      //dir end
      break;
    }
    else if(*((char *)d_entry) == '\xe5') {
      //unused
    }
    //Long filename text - Attrib has all four type bits set
    else {
      memset(filename, '\x00', D_ENTRY_NAME_SIZE + D_ENTRY_EXTENSION_SIZE + 1);
      strncpy(filename, d_entry->name, D_ENTRY_NAME_SIZE);
      strncpy(filename + strlen(filename), d_entry->extension, D_ENTRY_EXTENSION_SIZE);
      printf("d_entry:\nname: %s, size: %x, attribute: %x\n", buf + i, d_entry->size, d_entry->attribute);
    }
    
    i += sizeof(struct directory_entry);
  }

}