#ifndef __FAT32_H
#define __FAT32_H

#include <stdint.h>
#include "printf.h"
#include "sdhost.h"
#include "vfs.h"

#define ROUNDUP_MUL512(num) ((num + 511) & ~511)
#define JOIN_16(high, low) ((high << 16) | low)

#define SECTOR_SIZE 512

struct partition_entry {
    uint8_t status;
    uint8_t chs_first[3];
    uint8_t type;
    uint8_t chs_last[3];
    uint32_t lba_first;
    uint32_t num_sector;
} __attribute__((packed));

struct mbr {
    uint8_t boot_code[446];
    struct partition_entry p1;
    struct partition_entry p2;
    struct partition_entry p3;
    struct partition_entry p4;
    uint16_t signature;
} __attribute__((packed));

struct bios_parameter_blk {
    uint16_t bytes_per_logical_sector;
    uint8_t logical_sectors_per_cluster;
    uint16_t num_reserved_logical_sectors;
    uint8_t num_fat;
    uint16_t max_num_root_entries_12_16;
    uint16_t total_logical_sectors_12_16;
    uint8_t media_descriptor;
    uint16_t logical_sectors_per_fat_12_16;
    uint16_t physical_sectors_per_track;
    uint16_t num_of_heads_for_disks;
    uint32_t num_of_hidden_sectors;
    uint32_t total_logical_sectors;
    uint32_t logical_sectors_per_fat;
    uint16_t drive_description;
    uint16_t version;
    uint32_t cluster_num_of_root;
    uint8_t dum[42];
} __attribute__((packed));

struct boot_sector {
    uint8_t jump_instruction[3];
    uint64_t oem;
    struct bios_parameter_blk bpb;
    uint8_t dum[419];
    uint8_t physical_drive_num;
    uint16_t signature;
} __attribute__((packed));

#define FAT32_DIR 0b00010000

struct fat32_meta {
    uint32_t fat_begin_lba;
    uint32_t cluster_begin_lba;
    uint8_t sectors_per_cluster;
    uint32_t root_dir_first_cluster;
};

struct fat32_dir {
    char name[8];
    char ext[3];
    uint8_t attr;
    uint8_t dum0[8];
    uint16_t cluster_high;
    uint32_t dum1;
    uint16_t cluster_low;
    uint32_t size;
} __attribute__((packed));

static int setup_mount(struct filesystem *fs, struct mount *mount);
static int lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
//static int create(struct vnode* dir_node, struct vnode **target, const char *component_name, uint64_t mode);
//static int create_file(struct vnode* dir_node, struct vnode **target, const char *component_name);
//static int create_dir(struct vnode* dir_node, const char *component_name);
static int write(struct file *file, const void *buf, size_t len);
static int read(struct file *file, void *buf, size_t len);
static size_t filesize(struct file *file);
static void list(struct vnode *dir_node);
static void node_name(struct vnode *vnode, char *buf);
static void *content(struct vnode *vnode);

static void set_parent(struct vnode *mount, struct vnode *mountpoint);

static void dump_hex(void *buf, size_t size);
static uint32_t get_fat_value(struct fat32_meta *meta, uint32_t index);
static uint32_t lba_to_cluster(struct fat32_meta *meta, size_t lba);
static uint32_t cluster_to_lba(struct fat32_meta *meta, size_t cluster);
int32_t fat32_init();

#endif
