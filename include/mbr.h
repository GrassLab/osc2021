#ifndef _MBR_H
#define _MBR_H

#include "printf.h"

// The struct of partition of Master Boot Record (MBR) for disk
struct mbr_partition
{
    unsigned char status_flag;              //0x0
    unsigned char partition_begin_head;     //0x1
    unsigned short partition_begin_sector;  //0x2-0x3
    unsigned char partition_type;           //0x4
    unsigned char partition_end_head;       //0x5
    unsigned short partition_end_sector;    //0x6-0x7
    unsigned int starting_sector;           //0x8-0xB
    unsigned int nr_sector;                 //0xC-0xF
} __attribute__ ((packed));

static inline void dump_mbr_partition(struct mbr_partition *mbr_part)
{
    // You can check mbr spec here: 
    // https://tc.gts3.org/cs3210/2020/spring/r/fat-structs.pdf
    printf("============Dump MBR Partition============\n");
    printf("status_flag : 0x%x\n", mbr_part->status_flag);
    printf("Partition Type : 0x%x\n", mbr_part->partition_type);
    printf("starting_sector : 0x%x\n", mbr_part->starting_sector);
    printf("nr_sector : 0x%x\n", mbr_part->nr_sector);

}

#endif /* _MBR_H */