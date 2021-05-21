#include "fat32.h"
#include <printf.h>
#include <uart.h>
#include <string.h>
#include <varied.h>
#include <vfs.h>
#include <sdhost.h>

int fat32_filename_cmp(const char *component_name, const char *d_entry_name, uint32_t n) {
  
  for(int i = 0; i < n; i++) {
    if(component_name[i] >= 0x61 && component_name[i] <= 0x7a) {
      //lower case
      if(component_name[i] - 0x20 != d_entry_name[i]) {
        return 1;
      }  
    }
    else if(component_name[i] != d_entry_name[i]) {
      if(d_entry_name[i] != 0x20) {
        return 1;
      }
    }
    
  }
  return 0;
}

void fat32_filename_convert(char* name, char* extension, char* filename) {
  int i;
 
  memset(filename, FAT32_D_ENTRY_NAME_SIZE + FAT32_D_ENTRY_EXTENSION_SIZE + 1, '\0');
  i = 0; 
  for(int j = 0; j < FAT32_D_ENTRY_NAME_SIZE; j++) {
    if(name[j] != 0x20) 
      filename[i++] = name[j];
  }
  if(extension[0] != 0x20) {
    filename[i++] = '.';
    for(int j = 0; j < FAT32_D_ENTRY_EXTENSION_SIZE; j++) {
      filename[i++] = extension[j];
    }
  } 
}

static int lookup(struct vnode* dir_node, struct vnode** target, const char* component_name) {
  struct directory_entry* d_entry, *find_entry;
  struct fat32_inode *inode;
  struct vnode *v_node;
  char buf[FAT32_BLOCK_SIZE], fat_table[FAT32_BLOCK_SIZE], filename[FAT32_D_ENTRY_NAME_SIZE + FAT32_D_ENTRY_EXTENSION_SIZE + 1];
  size_t cluster_num, pre_cluster_num;
  int i, max_len;
  
  *target = null;

  if(component_name == null)
    return 1;
  if(dir_node == null)
    return 1;

  inode = dir_node->internal;
  
  if(inode == null)
    return 1;
  
  d_entry = &(inode->d_entry);
  //is directory
  if((d_entry->attribute) & 0x10 == 0) 
    return 1;

  //get first cluster number
  cluster_num = (d_entry->start_cluster_high << 16) + d_entry->start_cluster_low;
  pre_cluster_num = cluster_num;
  //read first fat table
  readblock(inode->lba + inode->num_of_reserved_sectors + cluster_num / FAT32_ENTRY_PER_FAT_TABLE, fat_table);
   
  while(1) {
    //read directory table in this cluster
    readblock(inode->lba + inode->num_of_reserved_sectors + inode->sectors_per_fat_large_fat32 * 2 + cluster_num - 2, buf);
    
    d_entry = (struct directory_entry *)buf;
    i = 0;
    //printf("cluster_num: %d\n", cluster_num);
    //traverse directory table
    while(d_entry + i <= buf + FAT32_BLOCK_SIZE) {
      if(*(char*)(d_entry + i) == '\x00') {
        //dir end
        break;
      }
      else if(*(char*)(d_entry + i) == '\xe5') {
        //unused
      }
      //Long filename text - Attrib has all four type bits set
      else {
        //sfn directory entry
        fat32_filename_convert((d_entry + i)->name, (d_entry + i)->extension, filename);
        //compare component name
        max_len = strlen(filename);
        if(max_len < strlen(component_name)) 
          max_len = strlen(component_name);
        if(fat32_filename_cmp(component_name, filename, max_len) == 0) {
          //create vnode, fat32_inode every time ?
          v_node = fat32_vnode_create(rootfs, d_entry + i, cluster_num, i);
          //printf("name: %s, size: %x, attribute: %x, first cluster: %x\n", filename, d_entry->size,  d_entry->attribute, cluster_num);
          *target = v_node;
          return 0;
        }
      }
      i += 1;
    }

   

    //check if need to read next fat table
    if(pre_cluster_num / FAT32_D_ENTRY_PER_D_TABLE != cluster_num / FAT32_D_ENTRY_PER_D_TABLE) {
      //need to read fat table
      readblock(inode->lba + inode->num_of_reserved_sectors + cluster_num / FAT32_ENTRY_PER_FAT_TABLE, fat_table);
    }
    
    //read next cluster
    pre_cluster_num = cluster_num;
    cluster_num = *(uint32_t *)(fat_table + (cluster_num % 128) * 4);

    //check if is last cluster
    if(IS_EOC(cluster_num)) {
      break;
    }
  }

  return 1;
}

static int setup_mount(struct filesystem* fs, struct mount* _mount) {
  struct directory_entry *d_entry;
  struct fat32_inode *inode;
  _mount->fs = fs;
  
   d_entry = (struct directory_entry* )varied_malloc(sizeof(struct directory_entry));

  if(d_entry == null) 
    return -1;

  //set root directory entry
  strncpy(d_entry->name, "/", 1);
  d_entry->start_cluster_high = 0;
  d_entry-> start_cluster_low = 2;
  d_entry->attribute = 0x10;
  
  _mount->root = fat32_vnode_create(_mount, d_entry, 0, 0);

  if(_mount->root == null) {
    return null;
  }
  
 /* struct vnode* v_node = (struct vnode *)varied_malloc(sizeof(struct vnode));
  lookup(_mount->root, &v_node, "rootfs");*/
        
  return 0;
}

void* fat32_vnode_create(struct mount* _mount, struct directory_entry* d_entry, size_t cluster_num, size_t offset) {
  struct vnode* v_node;
  struct fat32_inode * inode;
  v_node = (struct vnode* )varied_malloc(sizeof(struct vnode));

  if(v_node == null)
    return null;
  
  v_node->mount = _mount;
  v_node->f_ops = &fat32_fops;
  v_node->v_ops = &fat32_vops;
  
  inode = fat32_inode_create(current_partition, d_entry, cluster_num, offset);

  if(inode == null)
    return null;
  //set internal to fat32_inode
  v_node->internal = inode;
  
  return v_node;  
}

void* fat32_inode_create(struct fat32_info *fat32_info, struct directory_entry* d_entry, size_t cluster_num, size_t offset) {
   struct fat32_inode * inode;
  
  inode = (struct fat32_inode *)varied_malloc(sizeof(struct fat32_inode));

  if(inode == null)
    return null;
  
  memcpy(&inode->d_entry, d_entry, sizeof(struct directory_entry));
  inode->lba = fat32_info->p_entry->lba;
  inode->num_of_fats = fat32_info->boot_sector->num_of_fats;
  inode->num_of_reserved_sectors = fat32_info->boot_sector->num_of_reserved_sectors;
  inode->sectors_per_fat_large_fat32 = fat32_info->boot_sector->sectors_per_fat_large_fat32;
  inode->cluster_num_of_root_dir = fat32_info->boot_sector->cluster_num_of_root_dir;
  inode->cluster_num_of_d_entry = cluster_num;
  inode->d_entry_offset = offset;
  return inode;
}


size_t fat32_find_free_cluster(struct fat32_inode *inode) {
  char fat_table[FAT32_BLOCK_SIZE];
  uint32_t *fat_entry;

  for(int i = 0; i < inode->sectors_per_fat_large_fat32; i++) {
    readblock(inode->lba + inode->num_of_reserved_sectors + i, fat_table);
    
    fat_entry = fat_table;

    for(int j = 0; j < FAT32_ENTRY_PER_FAT_TABLE; j++) {
      //if next cluster num = 0, means free cluster
      if(*(fat_entry + j) == 0) {
        //set as last cluster
        *(fat_entry + j) = 0x0fffffff;
        writeblock(inode->lba + inode->num_of_reserved_sectors + i, fat_table);
        return i * FAT32_ENTRY_PER_FAT_TABLE + j;
      }
    }
  }

  return -1;
}


static int write(struct file* file, const void* buf, size_t len) {
  struct fat32_inode* inode;
  struct directory_entry *d_entry, *trav_d_entry;
  size_t write_bytes, cluster_num, pre_cluster_num, pos, size, write_len, cluster_size;
  char fat_buf[FAT32_BLOCK_SIZE], fat_table[FAT32_BLOCK_SIZE], filename[FAT32_D_ENTRY_NAME_SIZE + FAT32_D_ENTRY_EXTENSION_SIZE + 1];
  
  inode = file->vnode->internal;
  
  write_bytes = 0;

  if(inode == null)
    return write_bytes;
  
  d_entry = &(inode->d_entry);
  
  //is file
  if(d_entry->attribute != 0x20) {  
    return write_bytes;
  }

  //get first cluster number
  cluster_num = (d_entry->start_cluster_high << 16) + d_entry->start_cluster_low;
  pre_cluster_num = cluster_num;
  
  pos = file->f_pos;
  size = d_entry->size;
  cluster_size = FAT32_BLOCK_SIZE;

  while(1) {
    //read directory table in this cluster
    readblock(inode->lba + inode->num_of_reserved_sectors + inode->sectors_per_fat_large_fat32 * 2 + cluster_num - 2, fat_buf);

    if(pos < cluster_size) {
      //find position
      if(len <= cluster_size - pos) {
        
        memcpy(fat_buf + pos, (char *)buf + write_bytes, len);
        writeblock(inode->lba + inode->num_of_reserved_sectors + inode->sectors_per_fat_large_fat32 * 2 + cluster_num - 2, fat_buf);
        write_bytes += len;
        file->f_pos += write_bytes;
        
        break; 

      }
      else {
        write_len = cluster_size- pos; 
        memcpy(fat_buf + pos, (char *)buf + write_bytes, write_len);  
        writeblock(inode->lba + inode->num_of_reserved_sectors + inode->sectors_per_fat_large_fat32 * 2 + cluster_num - 2, fat_buf);
        len -= write_len;
        write_bytes += write_len; 
      }
    }
    else {
      //find next cluster
      pos -= cluster_size;
    }
    
    //check if need to read next fat table
    if(pre_cluster_num / FAT32_D_ENTRY_PER_D_TABLE != cluster_num / FAT32_D_ENTRY_PER_D_TABLE) {
      //need to read fat table
      readblock(inode->lba + inode->num_of_reserved_sectors + cluster_num / FAT32_ENTRY_PER_FAT_TABLE, fat_table);
    }

    //read next cluster
    pre_cluster_num = cluster_num;
    cluster_num = *(uint32_t *)(fat_table + (cluster_num % 128) * 4);

    //check if is last cluster
    if(IS_EOC(cluster_num)) {
      //need to allocate new space 
      cluster_num = fat32_find_free_cluster(inode);
      
      if(cluster_num == -1) 
        break;
      
      printf("new cluster num: %d\n", cluster_num);
      
      //update fat table
      *(uint32_t *)(fat_table + (pre_cluster_num % 128) * 4) = cluster_num;
      writeblock(inode->lba + inode->num_of_reserved_sectors + pre_cluster_num / FAT32_ENTRY_PER_FAT_TABLE, fat_table);
    }

   
  }
  //update directory entry
  if(file->f_pos - d_entry->size > 0) {
    d_entry->size += file->f_pos - d_entry->size;
    readblock(inode->lba + inode->num_of_reserved_sectors + inode->sectors_per_fat_large_fat32 * 2 + inode->cluster_num_of_d_entry - 2, fat_buf);
    ((struct directory_entry *)(fat_buf + FAT32_D_ENTRY_SIZE * inode->d_entry_offset))->size = d_entry->size;
    writeblock(inode->lba + inode->num_of_reserved_sectors + inode->sectors_per_fat_large_fat32 * 2 + inode->cluster_num_of_d_entry - 2, fat_buf);

  }
  return write_bytes;
}

static int read(struct file* file, void* buf, size_t len) {
  struct fat32_inode* inode;
  struct directory_entry *d_entry, *trav_d_entry;
  size_t read_bytes, cluster_num, pre_cluster_num, pos, read_len, cluster_size;
  char fat_buf[FAT32_BLOCK_SIZE], fat_table[FAT32_BLOCK_SIZE], filename[FAT32_D_ENTRY_NAME_SIZE + FAT32_D_ENTRY_EXTENSION_SIZE + 1];
  
  inode = file->vnode->internal;
  
  read_bytes = 0;

  if(inode == null)
    return read_bytes;
  
  d_entry = &(inode->d_entry);
  

  if(d_entry->attribute == 0x10) {
    //deal with ls [dir]

    if(file->f_pos == 0) {
      //initial 
      //file->f_pos, high 32 bytes store traverse cluster number
      //low 32 bytes store directory table offset
      cluster_num = (d_entry->start_cluster_high << 16) + d_entry->start_cluster_low;
      file->f_pos = cluster_num << 32;
      pos = 0;
    }
    else {
      cluster_num = file->f_pos >> 32;
      pos = (file->f_pos << 32) >> 32;
    }

    if(pos >= FAT32_D_ENTRY_PER_D_TABLE) {
      //read next cluster number
      readblock(inode->lba + inode->num_of_reserved_sectors + cluster_num / FAT32_ENTRY_PER_FAT_TABLE, fat_table);
      cluster_num = *(uint32_t *)(fat_table + (cluster_num % 128) * 4);
      
      if(IS_EOC(cluster_num)) {
        return read_bytes;
      }
      //update traverse cluster number
      file->f_pos = (cluster_num << 32) + pos;
    }

    //read directory table in this cluster
    readblock(inode->lba + inode->num_of_reserved_sectors + inode->sectors_per_fat_large_fat32 * 2 + cluster_num - 2, fat_buf);
    
    trav_d_entry = (struct directory_entry *)fat_buf + pos;
    if(*((char*)trav_d_entry) == '\x00') {
      //dir end
      return read_bytes;
    }
    else if(*((char*)trav_d_entry) == '\xe5') {
      //unused
    }
    //Long filename text - Attrib has all four type bits set
    else {
      fat32_filename_convert(trav_d_entry->name, trav_d_entry->extension, filename);
      strncpy(buf, filename, strlen(filename));
      read_bytes += strlen(filename);
    }
    
    file->f_pos += 1;
    
    /*cluster_num = file->f_pos >> 32;
    pos = (file->f_pos << 32) >> 32;
    printf("cluster num: %d, pos: %d\n", cluster_num, pos);*/
    return read_bytes;
  }
  
  if(d_entry->attribute != 0x20) {
    return read_bytes;
  }
  
  //check pos
  if(file->f_pos >= d_entry->size)
    return read_bytes;

  //need to update d_entry
  readblock(inode->lba + inode->num_of_reserved_sectors + inode->sectors_per_fat_large_fat32 * 2 + inode->cluster_num_of_d_entry - 2, fat_buf);
  memcpy(d_entry, ((struct directory_entry *)(fat_buf + FAT32_D_ENTRY_SIZE * inode->d_entry_offset)), sizeof(struct directory_entry));
   
  //get first cluster number
  cluster_num = (d_entry->start_cluster_high << 16) + d_entry->start_cluster_low;
  pre_cluster_num = cluster_num;
  //read first fat table
  readblock(inode->lba + inode->num_of_reserved_sectors + cluster_num / FAT32_ENTRY_PER_FAT_TABLE, fat_table);
  
  pos = file->f_pos;
  cluster_size = FAT32_BLOCK_SIZE;

  while(1) {
    //read directory table in this cluster
    readblock(inode->lba + inode->num_of_reserved_sectors + inode->sectors_per_fat_large_fat32 * 2 + cluster_num - 2, fat_buf);
    
    //check if need to read next fat table
    if(pre_cluster_num / FAT32_D_ENTRY_PER_D_TABLE != cluster_num / FAT32_D_ENTRY_PER_D_TABLE) {
      //need to read fat table
      readblock(inode->lba + inode->num_of_reserved_sectors + cluster_num / FAT32_ENTRY_PER_FAT_TABLE, fat_table);
    }
    //read next cluster
    pre_cluster_num = cluster_num;
    cluster_num = *(uint32_t *)(fat_table + (cluster_num % 128) * 4);
    
    //check if is last cluster
    if(IS_EOC(cluster_num) && pos < d_entry->size % FAT32_BLOCK_SIZE) {
      cluster_size = d_entry->size % FAT32_BLOCK_SIZE;
    }
    
    if(pos < cluster_size) {
      //find position
      if(len <= cluster_size - pos) {
        
        memcpy((char *)buf + read_bytes, fat_buf + pos, len);
        read_bytes += len;
        file->f_pos += read_bytes;
        break; 

      }
      else {
        read_len = cluster_size- pos; 
        memcpy((char *)buf + read_bytes, fat_buf + pos, read_len);  

        len -= read_len;
        read_bytes += read_len; 
      }
    }
    else {
      //find next cluster
      pos -= cluster_size;
     
    }
    
    //check if is last cluster
    if(IS_EOC(cluster_num)) {
      break;
    }
  }

  return read_bytes;
}

static int create(struct vnode* dir_node, struct vnode** target, const char* component_name) {

  return 0;
}

void fat32_init() {
  fat32_parse_mbr();

  for(int i = 0; i < 4; i++) {
    if(_mbr.partitions[i].lba != 0) {
      
      fat32_info_list[i].p_entry = &_mbr.partitions[i];
      fat32_info_list[i].boot_sector = fat32_parse_boot_sector(_mbr.partitions[i].lba);
      
      if(fat32_info_list[i].boot_sector != null) {
        fat32_parse_root_directory(&fat32_info_list[i]);
        fat32_traverse_root_directory(&fat32_info_list[i]);
        current_partition = &fat32_info_list[i];
        //test_read_file1(&fat32_info_list[i]);
      }
    }
  }
  
  //regist
  fat32_fops.read = read;
  fat32_fops.write = write;
  fat32_vops.create = create;
  fat32_vops.lookup = lookup;
  
  struct filesystem fat32_fs;
  fat32_fs.name = "fat32";
  fat32_fs.setup_mount = setup_mount;

  register_filesystem(&fat32_fs);  
}

void fat32_parse_mbr() {
  char buf[FAT32_BLOCK_SIZE];
  
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

void* fat32_parse_boot_sector(uint32_t lba) {
  char buf[FAT32_BLOCK_SIZE];
  
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
  
  printf("addr: %x\n", buf);
  readblock(lba + boot_sector->num_of_reserved_sectors, buf);
  return boot_sector;
}

void fat32_parse_root_directory(struct fat32_info* fat32_info) {
  char buf[FAT32_BLOCK_SIZE];
  struct directory_entry* d_entry;
  struct directory_table* d_table;
  char filename[FAT32_D_ENTRY_NAME_SIZE + FAT32_D_ENTRY_EXTENSION_SIZE + 1];
  int i;
  
  //root directory
  readblock(fat32_info->p_entry->lba + fat32_info->boot_sector->num_of_reserved_sectors + fat32_info->boot_sector->sectors_per_fat_large_fat32 * 2, buf);
  
  i = 0;
  while(1) {
    
    if(*(buf + i) == '\x00') {
      //dir end
      i += FAT32_D_ENTRY_SIZE;
      break;
    }
    else if(*(buf + i) == '\xe5') {
      //unused
    }
    //Long filename text - Attrib has all four type bits set
    else {
    }
    
    i += FAT32_D_ENTRY_SIZE;
  }
  
  d_entry = (struct directory_entry* )varied_malloc(i);

  if(d_entry == null)
    return;
  
  memcpy(d_entry, buf, i);
  
  d_table = (struct directory_table* )varied_malloc(sizeof(struct directory_table));

  if(d_table == null)
    return;

  d_table->root_entry = d_entry;
  d_table->size = i;

  fat32_info->d_table = d_table;
}

void fat32_traverse_root_directory(struct fat32_info* fat32_info) {
  char buf[FAT32_BLOCK_SIZE];
  struct directory_entry* d_entry;
  char filename[FAT32_D_ENTRY_NAME_SIZE + FAT32_D_ENTRY_EXTENSION_SIZE + 1];
  
  size_t cluster_num;
  d_entry = fat32_info->d_table->root_entry;
  while(1) {
    
    if(*((char*)d_entry) == '\x00') {
      //dir end
      break;
    }
    else if(*((char*)d_entry) == '\xe5') {
      //unused
      printf("unused\n");
      
    }
    //Long filename text - Attrib has all four type bits set
    else {
      memset(filename, '\x00', FAT32_D_ENTRY_NAME_SIZE + FAT32_D_ENTRY_EXTENSION_SIZE + 1);
      strncpy(filename, d_entry->name, FAT32_D_ENTRY_NAME_SIZE);
      strncpy(filename + strlen(filename), d_entry->extension, FAT32_D_ENTRY_EXTENSION_SIZE);
      cluster_num = (d_entry->start_cluster_high << 16) +  d_entry->start_cluster_low;
      printf("d_entry:\nname: %s, size: %x, attribute: %x, first cluster: %x\n", d_entry->name, d_entry->size,  d_entry->attribute, cluster_num);
    }
    
    d_entry += 1;
  }
 }

