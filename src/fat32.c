#include "fat32.h"
#include "vfs.h"
#include "printf.h"
#include "list.h"
#include "string.h"
#include "mm.h"
#include "types.h"
#include "sdhost.h"

struct vnode_operations *fat32_v_ops = NULL;
struct file_operations *fat32_f_ops = NULL;

struct fat32_metadata fat32_metadata; // For sd card fat32 metadata

static inline uint32_t _get_cluster_blk_idx(unsigned int cluster_index)
{
    return fat32_metadata.data_region_blk_idx +
           (cluster_index - fat32_metadata.rootDir_first_cluster) * fat32_metadata.sector_per_cluster;
}

// Given cluster idx, return block(sector) idx of FAT table entry
static inline uint32_t _get_FAT_entry_blk_idx(unsigned int cluster_index)
{
    return fat32_metadata.fat_region_blk_idx + (cluster_index / FAT32_ENTRY_PER_BLOCK);
}

// Given fat entry(blk idx and entry idx) then return corresponding cluster num
static inline uint32_t _get_cluster_num_by_fat_entry(int *free_FAT_entry_blk_idx, int *entry_idx)
{
    return fat32_metadata.rootDir_first_cluster + 
           (*free_FAT_entry_blk_idx - fat32_metadata.fat_region_blk_idx)*FAT32_ENTRY_PER_BLOCK + 
           *entry_idx;
}

int _find_next_cluster_blk_idx(uint32_t cur_cluster_num)
{
    int FAT_table[FAT32_ENTRY_PER_BLOCK];
    uint32_t next_cluster_num = 0;
    uint32_t target_FAT_block_idx = _get_FAT_entry_blk_idx(cur_cluster_num);
    readblock(target_FAT_block_idx, (void *)FAT_table);
    next_cluster_num = FAT_table[cur_cluster_num % FAT32_ENTRY_PER_BLOCK];

    #ifdef __FS_DEBUG
    for (int i = 0;i < FAT32_ENTRY_PER_BLOCK;i++) {
        printf("%d |", FAT_table[i]);
    }
    printf("\n");
    printf("[_find_next_cluster_blk_idx] fat32_metadata.fat_region_blk_idx = %d\n", fat32_metadata.fat_region_blk_idx);
    printf("[_find_next_cluster_blk_idx] cur_cluster_num = %d\n", cur_cluster_num);
    printf("[_find_next_cluster_blk_idx] target_FAT_block_idx = %u\n", target_FAT_block_idx);
    printf("[_find_next_cluster_blk_idx] next_cluster_num = 0x%x\n", next_cluster_num);
    #endif //__DEBUG

    return next_cluster_num;
}

int _get_empty_FAT_table_entry(int *free_FAT_entry_blk_idx, int *entry_idx)
{
    int isFound = FALSE;
    int FAT_table[FAT32_ENTRY_PER_BLOCK];

    // Iteratively search FAT table
    // Here only search first FAT table
    uint32_t fat_region_blk_idx = fat32_metadata.fat_region_blk_idx;
    for (int i = 0;i < fat32_metadata.sector_per_fat;i++) {
        int cur_fat_blk_idx = fat_region_blk_idx + i;
        readblock(fat_region_blk_idx + i, (void *)FAT_table);

        // Find if there are free fat entry in this block (sector)
        for (int j = 0;j < FAT32_ENTRY_PER_BLOCK;j++) {
            if (FAT_table[j] == 0x00) {
                isFound = TRUE;
                *free_FAT_entry_blk_idx = cur_fat_blk_idx;
                *entry_idx = j;

                printf("[_get_empty_FAT_table_entry] Find out empty FAT table entry\n");
                printf("[_get_empty_FAT_table_entry] free_FAT_entry_blk_idx = %d\n", *free_FAT_entry_blk_idx);
                printf("[_get_empty_FAT_table_entry] entry_idx = %d\n", *entry_idx);
                
                break;
            }    
        }

        if (isFound) break;
    }

    return isFound;     
}

int _set_FAT_table_entry(int *free_FAT_entry_blk_idx, int *entry_idx, uint32_t val)
{
    int FAT_table[FAT32_ENTRY_PER_BLOCK];
    readblock(*free_FAT_entry_blk_idx, (void *)FAT_table);

    FAT_table[*entry_idx] = val;

    return TRUE;
}

int _get_empty_FAT_dir_entry_by_clusterNum(uint32_t cluster_num, int *free_FAT_dir_entry_blk_idx, int *entry_idx)
{
    int isFound = FALSE;
    char sector_buf[BLOCK_SIZE];

    // Iteratively search FAT Directory block(secotr)
    // Here we only search free directory first cluster for easy.
    *free_FAT_dir_entry_blk_idx = _get_cluster_blk_idx(cluster_num);
    readblock(*free_FAT_dir_entry_blk_idx, sector_buf);
    struct fat32_dirEnt *sector_dirEnt = (struct fat32_dirEnt *)sector_buf;
    for (int i = 0;i < FAT32_DIRECTORY_ENTRY_PER_BLOCK;i++) {
        // Special value, it's a empty entry
        printf("[_get_empty_FAT_dir_entry_by_clusterNum] sector_dirEnt[%d].name[0] = 0x%x\n", i, sector_dirEnt[i].name[0] );
        if (sector_dirEnt[i].name[0] == 0x00) {
            isFound = TRUE;
            *entry_idx = i;
            break;
        }
    }
    
    printf("[_get_empty_FAT_dir_entry_by_clusterNum] free_FAT_dir_entry_blk_idx = %d\n", *free_FAT_dir_entry_blk_idx);
    return isFound;
}

int _set_FAT_dir_entry(int *free_FAT_dir_entry_blk_idx, int *dir_entry_idx, struct fat32_dirEnt *new_fat_dirEnt)
{
    char sector_buf[BLOCK_SIZE];
    readblock(*free_FAT_dir_entry_blk_idx, sector_buf);
    struct fat32_dirEnt *sector_dirEnt = (struct fat32_dirEnt *)sector_buf;
    sector_dirEnt[*dir_entry_idx] = *new_fat_dirEnt;
    
    // write back to sd card
    writeblock(*free_FAT_dir_entry_blk_idx, sector_buf);

    return TRUE;
}

struct fat32_dirEnt *_create_fat32_dirEnt(const char *filename, uint8_t FileAttributes, uint32_t new_first_cluster_num, uint32_t fileSize)
{
    struct fat32_dirEnt *new_fat_dirEnt = (struct fat32_dirEnt *) kmalloc(sizeof(struct fat32_dirEnt));
    memzero((unsigned long)new_fat_dirEnt, sizeof(struct fat32_dirEnt));

    /* Config directory entry */
    // parse Short file name (padded with spaces)
    int filename_len = strlen(filename);
    int cur_f_idx = 0;
    for (int i = 0;i < 8;i++) {
        if (filename[cur_f_idx] == '.')
            new_fat_dirEnt->name[i] = ' ';
        else {
            new_fat_dirEnt->name[i] = filename[cur_f_idx++];
        }
    }
    cur_f_idx++; // skip dot
    // Short file extension (padded with spaces)
    for (int i = 8;i < 11;i++) {
        if (i > filename_len) {
            new_fat_dirEnt->name[i] = ' ';
        }
        new_fat_dirEnt->name[i] = filename[cur_f_idx++];
    }
    // Other field 
    new_fat_dirEnt->attr = FileAttributes;
    new_fat_dirEnt->cluster_high = new_first_cluster_num >> 16;
    new_fat_dirEnt->cluster_low = new_first_cluster_num & 0xFFFF; 
    new_fat_dirEnt->size = fileSize;

    return new_fat_dirEnt;
}

struct dentry* fat32_create_dentry(struct dentry *parent, const char *name, int type)
{
    //printf("[fat32_create_dentry]\n");

    struct dentry *d = (struct dentry *) kmalloc(sizeof(struct dentry));
    
    strcpy(d->name, name);
    d->parent = parent;
    d->vnode = fat32_create_vnode(d, type);
    INIT_LIST_HEAD(&d->list);
    INIT_LIST_HEAD(&d->sub_dirs);
    // set parent's child list point to this new created dentry
    if (parent != NULL) {
        list_add(&d->list, &parent->sub_dirs);
    }
    
    d->mount = NULL;
    
    return d;
}

struct vnode* fat32_create_vnode(struct dentry *dentry, int type)
{
    //printf("[tmpfs_create_vnode]\n");
    struct vnode *vnode = (struct vnode*) kmalloc(sizeof(struct vnode));
    vnode->dentry = dentry;
    vnode->v_type = type;
    vnode->v_ops = fat32_v_ops;
    vnode->f_ops = fat32_f_ops;
    vnode->internal = NULL;
    
    return vnode;
}


int fat32_register()
{
    if (fat32_v_ops != NULL && fat32_f_ops != NULL) {
        return FAT32_ERROR; // Register error
    }

    // vnode operations
    fat32_v_ops = (struct vnode_operations *) kmalloc(sizeof(struct vnode_operations));
    fat32_v_ops->lookup = fat32_lookup;
    fat32_v_ops->create = fat32_create;
    fat32_v_ops->mkdir = fat32_mkdir;

    // file operations
    fat32_f_ops = (struct file_operations *) kmalloc(sizeof(struct file_operations));
    fat32_f_ops->write = fat32_write;
    fat32_f_ops->read = fat32_read;

    return 0;
}

int fat32_setup_mount(struct filesystem* fs, struct mount* mount, const char *component_name)
{
    // printf("[fat32_setup_mount]\n");
    mount->fs = fs;
    mount->root = fat32_create_dentry(NULL, component_name, DIRECTORY);
    
    printf("[fat32_setup_mount] Setup tmpfs, New created dentry name = %s\n", mount->root->name);

    return TRUE;
}

/** 
 * load all subdirectory under specified directory(dir_node) from sdcard device 
 */
void _fat32_load_dentry_from_device(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    printf("[_fat32_try_load_dentry_from_device]\n");

    // Read first block of first cluster 
    uint8_t sector_buf[BLOCK_SIZE];
    struct fat32_internal *dir_internal= (struct fat32_internal *) dir_node->internal;
    uint32_t dirEntry_blk_idx = _get_cluster_blk_idx(dir_internal->first_cluster);
    readblock(dirEntry_blk_idx, sector_buf);
    
    #ifdef __FS_DEBUG
    _dump_fat32_internal(dir_internal);
    #endif

    struct fat32_dirEnt *sector_dirEnt = (struct fat32_dirEnt *)sector_buf;
    for (int i = 0;sector_dirEnt[i].name[0] != 0x00 && i < FAT32_DIRECTORY_ENTRY_PER_BLOCK;i++) { // Search until special value (0x00 for first byte) as fat23 spec
        // Special value, it's a deleted/unused entry
        if (sector_dirEnt[i].name[0] == 0xE5) {
            continue;
        }

        // Extract filename
        char filename[13 + 1]; // 13 (SFN) + 1 ('\0')
        int f_len = 0;
        for (int j = 0;j < 8;j++) {
            char c = sector_dirEnt[i].name[j];
            if (c == ' ') { // end
                break;
            }
            filename[f_len++] = c;
        }
        filename[f_len++] = '.';
        for (int j = 0;j < 3;j++) {
            char c = sector_dirEnt[i].ext[j];
            if (c == ' ') {
                break;
            }
            filename[f_len++] = c;
        }
        filename[f_len++] = '\0';

        // Check if file (dentry) have been loaded in memory
        struct dentry *dir_dentry = dir_node->dentry;
        struct dentry *pos;
        int isDentryExist = FALSE;
        list_for_each_entry(pos, &dir_dentry->sub_dirs, list) {
            #ifdef __FS_DEBUG
            printf("[fat32_lookup] pos->name = %s\n", pos->name);
            #endif //__DEBUG
            if (!strcmp(pos->name, filename)) {
                *target = pos->vnode;
                isDentryExist = TRUE;
                break;
            }
        }
        if (isDentryExist) {
            #ifdef __FS_DEBUG
            printf("[_fat32_load_dentry_from_device] '%s' exist!\n", filename);
            #endif //__DEBUG
            continue;
        }

        // If file(directory or regular file) not exist, create a new one in memory
        struct dentry *new_dentry = NULL;
        if (sector_dirEnt[i].attr == DIRECTORY_ENTRY_ATTR_DIRECTORY)
            new_dentry = fat32_create_dentry(dir_node->dentry, filename, DIRECTORY);
        else
            new_dentry = fat32_create_dentry(dir_node->dentry, filename, REGULAR_FILE);

        #ifdef __FS_DEBUG
        printf("Seach directory entry in sector | i = %d, filename = %s\n", i, filename);
        #endif

        // assign fat32 internal info for the new child dentry 
        struct fat32_internal *internal = (struct fat32_internal *) kmalloc(sizeof(struct fat32_internal));
        internal->first_cluster = (sector_dirEnt[i].cluster_high << 16) | (sector_dirEnt[i].cluster_low);
        internal->dirEntry_blk_idx = dirEntry_blk_idx;
        internal->size = sector_dirEnt[i].size;

        new_dentry->vnode->internal = internal;

        #ifdef __FS_DEBUG
        _dump_fat32_internal(internal);
        #endif
    } 

    return;
}

int fat32_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    #ifdef __FS_DEBUG
    printf("[fat32_lookup]\n");
    #endif //__DEBUG

    int isNextVnodeFound = FALSE;
    struct dentry *dir_dentry = dir_node->dentry;
    struct dentry *pos;
    
    list_for_each_entry(pos, &dir_dentry->sub_dirs, list) {
        #ifdef __FS_DEBUG
        printf("[fat32_lookup] pos->name = %s\n", pos->name);
        #endif //__DEBUG
        if (!strcmp(pos->name, component_name)) {
            *target = pos->vnode;
            isNextVnodeFound = TRUE;
            break;
        }
    }

    if (!isNextVnodeFound) // If next vnode not found
    {
        // load all subdirectory under specify directory from sdcard device 
        _fat32_load_dentry_from_device(dir_node, target, component_name);

        // retry to find the next vnode 
        list_for_each_entry(pos, &dir_dentry->sub_dirs, list) {
            #ifdef __FS_DEBUG
            printf("[fat32_lookup] pos->name = %s\n", pos->name);
            #endif //__FS_DEBUG
            if (!strcmp(pos->name, component_name)) {
                *target = pos->vnode;
                isNextVnodeFound = TRUE;
                break;
            }
        }
    }

    return isNextVnodeFound;

}

int fat32_read(struct file *file, void *buf, size_t len)
{
    #ifdef __FS_DEBUG
    printf("[fat32_read] file->f_pos = %d\n", file->f_pos);
    #endif //__DEBUG

    char *dest = (char *)buf;

    char sector_buf[BLOCK_SIZE];
    struct fat32_internal *internal = file->vnode->internal;
    uint32_t contentSize = internal->size;
    size_t cur_read_count = 0; // record how many bytes have been read

    uint32_t current_cluster_num = internal->first_cluster;
    for (int i = 0;i < file->f_pos / BLOCK_SIZE;i++) { // get the right cluster to start with for file read 
        current_cluster_num = _find_next_cluster_blk_idx(current_cluster_num);
    }

    // read until meet len needed to write, reach file's content size, or reach end-of-cluster-chain marker 
    int isMeetReadLen = (cur_read_count >= len);
    int isReachContentSize = file->f_pos >= contentSize;
    int isReachEndOfCluster = FALSE; //  Maybe it's redundant. Same logic as isReachContentSize here?
    while (!isMeetReadLen && !isReachContentSize && !isReachEndOfCluster) {
        
        // read data from sd carddevice
        readblock(_get_cluster_blk_idx(current_cluster_num), sector_buf);
        
        // Calculate how many char this iteration we needed to read
        // We need to determine how many bytes we still can read in this iteration, choose the smallest one in following:
        // 1. How many char we still want to read
        // 2. How many char we still can read from this block (sector)
        // 3. How many byte left in entire file. 
        uint32_t charStillNeedReadSize = len - cur_read_count; // 1.
        uint32_t contentLeftInCurBlock = BLOCK_SIZE - (file->f_pos % BLOCK_SIZE); // 2.
        uint32_t remainingContectSize = contentSize - file->f_pos; // 3.
        uint32_t charNeeded = (charStillNeedReadSize < contentLeftInCurBlock) ? charStillNeedReadSize : contentLeftInCurBlock;
        charNeeded = (charNeeded > remainingContectSize) ? remainingContectSize: charNeeded; 

        #ifdef __FS_DEBUG
        printf("[fat32_read] contentSize= %d\n", contentSize);
        printf("[fat32_read] charNeeded = %d\n", charNeeded);
        #endif 

        // copy content to buf
        int sector_buf_start_idx = file->f_pos % BLOCK_SIZE;
        for (int i = 0;i < charNeeded;i++) {
            #ifdef __FS_DEBUG
            if (i % 10 == 0)
                printf("[fat32_read] i = %d, ch = %c\n", i, sector_buf[sector_buf_start_idx + i]);
            #endif 
            dest[cur_read_count++] = sector_buf[sector_buf_start_idx + i];
        }

        // update to next cluster
        current_cluster_num = _find_next_cluster_blk_idx(current_cluster_num);
        // update file position
        file->f_pos += charNeeded;

        // Check if read next block needed
        isMeetReadLen = cur_read_count >= len;
        isReachContentSize = (file->f_pos) >= contentSize;
        isReachEndOfCluster = (current_cluster_num == EOC_FILTER);
        #ifdef __FS_DEBUG
        printf("cur_read_count = %d\n", cur_read_count);
        printf("isMeetReadLen = %d\n", isMeetReadLen);
        printf("isMeetisReachEndOfClusterReadLen = %d\n", isReachEndOfCluster);
        printf("isReachContentSize = %d\n", isReachContentSize);
        #endif 
    }

    // After experiment, Linux append '\0' to file in device, so we don't need to add '\0' manully
    dest[cur_read_count] = '\0';

    return cur_read_count;
}

int fat32_write(struct file *file, const void *buf, size_t len)
{
    // TODO: Lab7 - Requirement2
    #ifdef __FS_DEBUG
    printf("[fat32_write] Content in buf : %s\n", buf);
    #endif //__DEBUG

    char *src = (char *)buf;
    char sector_buf[BLOCK_SIZE];

    struct fat32_internal *internal = file->vnode->internal;
    uint32_t contentSize = internal->size;
    int cur_write_count = 0; // record how many bytes have been written

    uint32_t current_cluster_num = internal->first_cluster;
    for (int i = 0;i < file->f_pos / BLOCK_SIZE;i++) { // get the right cluster to start with   
        current_cluster_num = _find_next_cluster_blk_idx(current_cluster_num);
    }

    // write until meet len needed to write
    int isMeetWriteLen = (cur_write_count >= len);
    while (!isMeetWriteLen) {
        // read data from sd card device
        readblock(_get_cluster_blk_idx(current_cluster_num), sector_buf);

        // Calculate how many char this iteration we needed to write
        // We need to determine how many bytes we still can write in this iteration, choose the smallest one in following:
        // 1. How many char we still want to write
        // 2. How many char we still can write into this block (sector)
        uint32_t charStillNeedWriteSize = len - cur_write_count;  // 1.
        uint32_t contentLeftInCurBlock = BLOCK_SIZE - (file->f_pos % BLOCK_SIZE);  // 2.
        uint32_t charNeeded = (charStillNeedWriteSize < contentLeftInCurBlock) ? charStillNeedWriteSize : contentLeftInCurBlock;

        // write content  
        int sector_buf_start_idx = file->f_pos % BLOCK_SIZE; 
        #ifdef __FS_DEBUG
        printf("[fat32_write] current_cluster_num= %d\n", current_cluster_num);
        printf("[fat32_write] contentSize= %d\n", contentSize);
        printf("[fat32_write] charNeeded = %d\n", charNeeded);
        printf("[fat32_write] sector_buf_start_idx = %d\n", sector_buf_start_idx);
        #endif
        for (int i = 0;i < charNeeded;i++) {
            sector_buf[sector_buf_start_idx + i] = src[cur_write_count++];
        }
        
        // sector content write back
        writeblock(_get_cluster_blk_idx(current_cluster_num), sector_buf);

        // update to next cluster
        uint32_t prev_cluster_num = current_cluster_num; // save prev for later uses
        current_cluster_num = _find_next_cluster_blk_idx(current_cluster_num);
        // update file position
        file->f_pos += charNeeded;

        // Check if write next block needed
        isMeetWriteLen = cur_write_count >= len;
        #ifdef __FS_DEBUG
        printf("isMeetReadLen = %d\n", isMeetWriteLen);
        #endif

        // If reach EOC but still chars needed to write, we need extra cluster to store remaining file content 
        int isReachEndOfCluster = (current_cluster_num == EOC_FILTER);
        if (isReachEndOfCluster && !isMeetWriteLen) {
            printf("[fat32_write] Extend new cluster not implementd. Write procedure terminate.\n");
            return -1;

            printf("prev_cluster_num = %d\n", prev_cluster_num);
            // TODO:
            // int free_FAT_entry_blk_idx;
            // int entry_idx;
            // int isFound = _get_empty_FAT_table_entry(&free_FAT_entry_blk_idx, &entry_idx);
            // if (isFound == FALSE) {
            //     printf("[fat32_write] Error, no free FAT table entry.\n");
            // }
        }

    }

    // File's content size is change (larger), update file size in direcotry dentry in file internal struct and device  
    int isChangeContentSize = (file->f_pos) > contentSize;
    if (isChangeContentSize) {
        internal->size = file->f_pos; // update file size in memory

        // find dir entry and update file size of directory entry in sd card
        readblock(internal->dirEntry_blk_idx, sector_buf);
        struct fat32_dirEnt *sector_dirEnt = (struct fat32_dirEnt *)sector_buf;
        for (int i = 0;sector_dirEnt[i].name[0] != 0x00 && i < FAT32_DIRECTORY_ENTRY_PER_BLOCK;i++) {
            // Special value, it's a deleted/unused entry
            if (sector_dirEnt[i].name[0] == 0xE5) 
                continue;

            // find target file directory entry
            uint32_t first_cluster = (sector_dirEnt[i].cluster_high) << 16 | sector_dirEnt[i].cluster_low;
            if (first_cluster == internal->first_cluster) {
                sector_dirEnt[i].size = file->f_pos;
            }
        }
        writeblock(internal->dirEntry_blk_idx, sector_buf); // write back
    }

    #ifdef __FS_DEBUG
    printf("isChangeContentSize = %d\n", isChangeContentSize);
    printf("file->f_pos = %d\n", file->f_pos);
    #endif

    return cur_write_count;
}

// Lab7 - Elective1, Create a File in FAT32
int fat32_create(struct vnode *dir_node, struct vnode **target, const char *component_name)
{
    struct dentry *new_dentry = fat32_create_dentry(dir_node->dentry, component_name, REGULAR_FILE);
    *target = new_dentry->vnode;

    /* Find free FAT table entry (first cluster) and free directory entry in sd card */
    // 1. Find an empty entry in the FAT table .
    int free_FAT_entry_blk_idx;
    int entry_idx;
    int isFound = _get_empty_FAT_table_entry(&free_FAT_entry_blk_idx, &entry_idx);
    if (isFound == FALSE) {
        printf("[fat32_write] Error, no free FAT table entry.\n");
        return FALSE;
    }

    // 2. Find an empty directory entry in the target directory.
    // find empty directory entry in directory's first cluster
    struct fat32_internal *internal = (struct fat32_internal *) dir_node->internal;
    int free_FAT_dir_entry_blk_idx;
    int dir_entry_idx = -1;
    isFound = _get_empty_FAT_dir_entry_by_clusterNum(internal->first_cluster, &free_FAT_dir_entry_blk_idx, &dir_entry_idx);
    if (isFound == FALSE) {
        printf("[fat32_create] Error, no free FAT direcotry entry (Here only search first cluster for easy).\n");
        return FALSE;
    }
    #ifdef __FS_DEBUG
    printf("[fat32_create] free_FAT_dir_entry_blk_idx = %d\n", free_FAT_dir_entry_blk_idx);
    printf("[fat32_create] dir_entry_idx = %d\n", dir_entry_idx);
    #endif

    // Create new directory entry
    uint32_t new_first_cluster_num = _get_cluster_num_by_fat_entry(&free_FAT_entry_blk_idx, &entry_idx); // Calculate new file's first cluster num
    struct fat32_dirEnt *new_fat_dirEnt = _create_fat32_dirEnt(component_name, 0x00, new_first_cluster_num, 0);
    printf("[fat32_create] new_first_cluster = %d\n", new_first_cluster_num);
    #ifdef __FS_DEBUG
    printf("[fat32_create] new_first_cluster = %d\n", new_first_cluster_num);
    #endif

    // 3. Save them to sd card. Also set vnode internal info in memory
    // set fat table entry in sdcard
    _set_FAT_table_entry(&free_FAT_entry_blk_idx, &entry_idx, EOC_FILTER); // Set to EOC_FILTER, because there are one cluster initially. 
    // set directory entry in sd card
    _set_FAT_dir_entry(&free_FAT_dir_entry_blk_idx, &dir_entry_idx, new_fat_dirEnt);

    // set new file vnode with fat32 internal info (In memory) 
    struct fat32_internal *new_internal = (struct fat32_internal *) kmalloc(sizeof(struct fat32_internal));
    new_internal->first_cluster = new_first_cluster_num;
    new_internal->dirEntry_blk_idx = free_FAT_dir_entry_blk_idx;
    new_internal->size = 0;
    new_dentry->vnode->internal = new_internal;

    return TRUE;
}

int fat32_mkdir(struct vnode *parent, const char *component_name)
{
    // TODO: Not needed for Lab7
    // printf("[fat32_mkdir] dir name: %s\n", component_name);
    // fat32_create_dentry(parent->dentry, component_name, DIRECTORY);

    return TRUE;
}
