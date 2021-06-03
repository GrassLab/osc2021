#ifndef __USER_LIB__
#define __USER_LIB__
#define NULL 0
#define MAP_ANONYMOUS 0
#define MM_TYPE_PAGE_TABLE		    0x3
#define MM_TYPE_PAGE 			    0x3
#define MM_TYPE_BLOCK			    0x1
#define MM_ACCESS			        (0x1 << 10)
#define MM_ACCESS_PERMISSION		(0x01 << 6) 
#define MM_READ_WRITE_PERMISSION    (0x1 << 7)
#define MM_EXACUTE_EL0_PERMISSION   (0x1 << 53)
#define MM_EXACUTE_EL1_PERMISSION   (0x1 << 54)
/*
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *			n	MAIR
 *   DEVICE_nGnRnE	000	00000000
 *   NORMAL_NC		001	01000100
 */
#define MT_DEVICE_nGnRnE 		0x0
#define MT_NORMAL_NC			0x1
#define MT_DEVICE_nGnRnE_FLAGS		0x00
#define MT_NORMAL_NC_FLAGS  		0x44
#define MAIR_VALUE			(MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))

#define MMU_FLAGS	 		(MM_TYPE_BLOCK | (MT_NORMAL_NC << 2) | MM_ACCESS)	
#define MMU_DEVICE_FLAGS		(MM_TYPE_BLOCK | (MT_DEVICE_nGnRnE << 2) | MM_ACCESS)	
#define MMU_PTE_FLAGS			(MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_PERMISSION)	

#define PROT_NONE   (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_READ_WRITE_PERMISSION | MM_EXACUTE_EL0_PERMISSION | MM_EXACUTE_EL1_PERMISSION)
#define PROT_READ   (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_READ_WRITE_PERMISSION | MM_ACCESS_PERMISSION | MM_EXACUTE_EL0_PERMISSION | MM_EXACUTE_EL1_PERMISSION)
#define PROT_READ_WRITE  (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_PERMISSION | MM_EXACUTE_EL0_PERMISSION | MM_EXACUTE_EL1_PERMISSION)
#define PROT_EXEC   (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_READ_WRITE_PERMISSION | MM_ACCESS_PERMISSION) 

#define REG_FILE 1
#define REG_DIR  2
#define O_CREAT  4

void init_print();
int strcmp(char *str1, char *str2);
unsigned long *mmap(unsigned long addr, unsigned long len, int prot, int flags, void* file_start, int file_offset);
#endif