#include "fat32.h"
#include "vfs.h"
#include "uart.h"
#include "string.h"
#include "allocator.h"
#include "sdhost.h"
#include "utils.h"

void read_partition_block(int block_idx, void* buf) {
    readblock(2048 + block_idx, buf);
}

void write_partition_block(int block_idx, void* buf) {
    writeblock(2048 + block_idx, buf);
}

void read_data_region(int cluster_num, void* buf) {
    read_partition_block(
        fat32_meta.num_reserved_sector + 
        fat32_meta.num_fat * fat32_meta.sectors_per_fat + 
        (cluster_num - fat32_meta.root_cluster_num), buf
    );
    /* 
    Q: Why minus fat32_meta.root_cluster_num?
    Ans: FAT32 typically commences the Root Directory Table in cluster number 2: the first cluster of the Data Region.
    */ 
}

void write_data_region(int cluster_num, void* buf) {
    write_partition_block(
        fat32_meta.num_reserved_sector + 
        fat32_meta.num_fat * fat32_meta.sectors_per_fat + 
        (cluster_num - fat32_meta.root_cluster_num), buf
    ); 
}

void parse_fname(char* sfn, char *sfe, char* fname) {
    int i = 0;
    for(; i < 8; i++) {
        fname[i] = sfn[i];
    }
    if(*sfe == ' ') {
        fname[i++] = '\0';
        return;
    }
    fname[i++] = '.';
    for(int j = 0; j < 3; j++) {
        fname[i++] = sfe[j];
    }
    fname[i] = '\0';
    return;
}

unsigned int get_fat_entry(int cluster_num) { 
    // one FAT entry spans four bytes 4 bytes(unsigned int contains 32 bits)
    unsigned int fat[fat32_meta.bytes_per_sector/4]; 
    // cluster_num indicate it is the (cluster_num)_th entry in FAT (except it is the next cluster in a chain)
    // we already known bytes per sector(block) is 512 bytes, and one FAT entry spans four bytes 4 bytes(in little-endian byte order!!), so a block contains (512/4 = 128) FAT entries
    // so cluster_num / (512/4) indicates it belongs to which sector(block)
    // thus, the offset of required FAT entry is num_reserved_sector + (cluster_num / (512/4)) 
    read_partition_block(fat32_meta.num_reserved_sector + cluster_num / (fat32_meta.bytes_per_sector/4), (unsigned char *)fat);
    // The four top bits of each entry are reserved for other purposes; they are cleared during formatting 
    // and should not be changed otherwise. They must be masked off before interpreting the entry as 28-bit cluster address.
    // find the cluster_num index in fat array
    return fat[cluster_num % (fat32_meta.bytes_per_sector/4)] & 0x0fffffff; 
}

void set_fat_entry(int cluster_num, unsigned int val) {
    unsigned int fat[fat32_meta.bytes_per_sector/4];
    read_partition_block(fat32_meta.num_reserved_sector + cluster_num / (fat32_meta.bytes_per_sector/4), (unsigned char *)fat);
    fat[cluster_num % (512/4)] = val;
    write_partition_block(fat32_meta.num_reserved_sector + cluster_num / (fat32_meta.bytes_per_sector/4), (unsigned char *)fat);
}

unsigned int get_new_cluster() { // find an available cluster
    for(int cluster_index = 0; ; cluster_index++) { // no limitation?
        // if the value of FAT entry is a zero, note that the cluster is unused
        if(get_fat_entry(cluster_index) == 0) { 
            set_fat_entry(cluster_index, EOC);
            return cluster_index;
        }
    }
}

void update_file_cluster_index(unsigned int dir_cluster_index, const char* component_name, unsigned int new_cluster_index) {
    char buf[512];
    read_data_region(dir_cluster_index, buf);
    for(char* cur = buf; *cur != '\0'; cur += DENTRY_SIZE) {
        char fname[13];
        struct  dir_entry_data* data;
        data = (void*) buf;
        
        parse_fname(cur, cur+8, fname);
        if(!strcmp(fname, component_name)) {
            //*((unsigned int *)(cur + 0x014)) = (new_cluster_index >> 16) & 0xffff;
            //*((unsigned int *)(cur + 0x01a)) = new_cluster_index & 0xffff;
            data->start_hi = (new_cluster_index >> 16) & 0xffff;
            data->start_lo = new_cluster_index & 0xffff; 
            write_data_region(dir_cluster_index, buf);
            return;
        }
    }
}

void update_file_size(unsigned int dir_cluster_index, const char* component_name, unsigned int new_size) {
    unsigned char buf[512];
    read_data_region(dir_cluster_index, buf);
    
    for(unsigned char* cur = buf; *cur != '\0'; cur += DENTRY_SIZE) {
        char fname[13];
        parse_fname(cur, cur+8, fname);
        if(!strcmp(fname, component_name)) {
            *((unsigned int *)(cur + 0x01c)) = new_size;
            write_data_region(dir_cluster_index, buf);
            return;
        }
    }
}

int fat32_write(file* file, const void* buf, unsigned long len) {
    dentry* content = file->vnode->internal;
    int f_pos = file->f_pos;
    
    int entry_level = f_pos / 512;
    unsigned int cluster_index = content->cluster_index;
    for(int l = 0; l < entry_level; l++) {
        cluster_index = get_fat_entry(cluster_index);
    }
    /*
    // if file size == 0, allocate a cluster for it
    if(content->size == 0) {
        cluster_index = get_new_cluster();
        update_file_cluster_index(content->parent_cluster_index, content->name, cluster_index);
        content->cluster_index = cluster_index;
    }
    */
    
    int iter = f_pos % 512;
    unsigned char write_buf[512];
    int len_write = 0;
    
    for(; len_write < len; f_pos++, len_write++) {
        write_buf[iter] = ((unsigned char*)buf)[len_write];

        if(++iter % 512 == 0) {
            write_data_region(cluster_index, write_buf);
            iter = 0;
            cluster_index = get_fat_entry(cluster_index);
            // reach the last cluster in file, we should extend new cluster for it
            if(cluster_index == EOC) { 
                unsigned int new_cluster_index = get_new_cluster();
                set_fat_entry(cluster_index, new_cluster_index);
                cluster_index = new_cluster_index;
            }
            //read_data_region(cluster_index, write_buf); //I think there is no need to read?
        }
    }
    write_data_region(cluster_index, write_buf); // for the case that iter%512 != 0 in the last loop

    if(f_pos > content->size) {
        update_file_size(content->parent_cluster_index, content->name, f_pos);
        content->size = f_pos;
    }
    
    //file->f_pos = f_pos;
    return len_write;
}

int fat32_read(file* file, void* buf, unsigned long len) {
    dentry* content = file->vnode->internal;
    int f_pos = file->f_pos;
    if(f_pos > content->size) {
        return -1; // invalid read
    }

    int entry_level = f_pos / 512;
    unsigned int cluster_index = content->cluster_index;
    for(int l = 0; l < entry_level; l++) {
        cluster_index = get_fat_entry(cluster_index);
    }

    int iter = f_pos % 512;
    unsigned char read_buf[512];
    read_data_region(cluster_index, read_buf);
    int len_read = 0;
    for(; f_pos < content->size && len_read < len; f_pos++, len_read++) {
        ((unsigned char*)buf)[len_read] = read_buf[iter];
        if(++iter % 512 == 0) {
            iter = 0;
            cluster_index = get_fat_entry(cluster_index);
            read_data_region(cluster_index, read_buf);
        }
    }
    file->f_pos = f_pos;
    return len_read;
}

int fat32_create() {
    return 0;
}

int fat32_lookup(vnode* dir_node, vnode** target, const char* component_name) {
    dentry* content = (dentry*)dir_node->internal;
    unsigned char buf[BLOCK_SIZE];

    read_data_region(content->cluster_index, buf);
    for(char* cur = (char*)buf; *cur != '\0'; cur += DENTRY_SIZE) {
        char fname[13];
        struct dir_entry_data *data;
        data = (void*) cur;
        
        parse_fname(cur, cur+8, fname);
        if(!strcmp(fname, component_name)) {
            // uart_puts("find!\n");
            vnode* node = (vnode*) kmalloc(sizeof(vnode));
            dentry* content = (dentry*) kmalloc(sizeof(dentry));

            content->vnode = node;
            // content->attribute = cur[0x0b];
            // content->cluster_index = *((unsigned int*)(cur + 0x1a)) + (((unsigned int)*((unsigned short*)(cur + 0x14))) << 16);
            // content->size = *((unsigned int*)(cur + 0x1c));
            // content->attribute = "0";
            content->cluster_index = data->start_lo + (unsigned int)(data->start_hi << 16);
            content->size = data->size;
            content->name = fname;
            
            node->f_ops = dir_node->f_ops;
            node->mount = dir_node->mount;
            node->v_ops = dir_node->v_ops;
            node->internal = content;
            content->parent_cluster_index = ((dentry*)(dir_node->internal))->cluster_index;
            
            if(target) {
                *target = node;
            }
            return 0;
        }
    }
    *target = NULL;
    return -1;
}

int fat32_init(mount* mnt, vnode* root) {
    unsigned char buf[512] __attribute__ ((aligned(0x10)));;
    read_partition_block(0, buf); // base = 2048 in read_partition_block
    
    /*
    dereferenc does not work on pi(maybe is the alignment problem?)
    fat32_meta.bytes_per_sector = 512;
    fat32_meta.sectors_per_cluster = *((unsigned char *)(buf+0x00d));
    fat32_meta.num_reserved_sector = *((unsigned short*)(buf+0x00e));
    fat32_meta.num_fat = *((unsigned char*)(buf+0x010));
    fat32_meta.sectors_per_fat = *((unsigned int*)(buf+0x024));
    fat32_meta.root_cluster_num = *((unsigned int*)(buf+0x02c)); // cluster number of root directory start
    */
    
    struct boot_sector *boot_sector;
    boot_sector = (void*) buf;
    fat32_meta.bytes_per_sector = ((unsigned short)(boot_sector->bytes_per_sector_hi) << 8) + (unsigned short)boot_sector->bytes_per_sector_lo;
    fat32_meta.sectors_per_cluster = boot_sector->sectors_per_cluster;
    
    fat32_meta.num_reserved_sector = boot_sector->count_of_reserved;
    fat32_meta.num_fat = boot_sector->num_of_fat;
    fat32_meta.sectors_per_fat = boot_sector->sectors_per_fat;
    fat32_meta.root_cluster_num = boot_sector->cluster_num_of_root; // cluster number of root directory start
    
    
    uart_printf("[System info]: print metadata of FAT32...\n");
    uart_printf("bytes per secotr: %d\n", fat32_meta.bytes_per_sector);
    uart_printf("sector per cluster: %d\n", fat32_meta.sectors_per_cluster);
    uart_printf("number of sectors: %d\n", fat32_meta.num_reserved_sector);
    uart_printf("number of fat: %d\n", fat32_meta.num_fat);
    uart_printf("sectors per fat: %d\n", fat32_meta.sectors_per_fat);
    uart_printf("number of root cluster: %d\n", fat32_meta.root_cluster_num);

    root->mount = mnt;
    root->v_ops = (vnode_operations*) kmalloc(sizeof(vnode_operations));
    root->v_ops->lookup = fat32_lookup;
    root->v_ops->create = fat32_create;
    root->f_ops = (file_operations*) kmalloc(sizeof(file_operations));
    root->f_ops->write = fat32_write;
    root->f_ops->read = fat32_read;
    
    
    dentry* content = (dentry*)kmalloc(sizeof(dentry));
    content->cluster_index = fat32_meta.root_cluster_num;
    content->vnode = root;
    
    root->internal = content;
    return 1;
}

int fat32_setup(filesystem* fs, mount* mnt) {
    uart_puts("[System info]: setup filesystem as fat32...\n");
    sd_init();
    mnt->root = (vnode*) kmalloc(sizeof(vnode));
    mnt->fs = fs;
    if(!fat32_init(mnt, mnt->root)) {
        uart_printf("fat32_init error!\n");
        return 1;
    }
    return 0;
}