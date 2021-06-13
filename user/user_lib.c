#include "./printf.h"
#include "./user_lib.h"

#define VA_START 			0xffff000000000000
#define DEVICE_BASE 		0x3F000000	
#define PBASE 			(VA_START + DEVICE_BASE)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)
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
#define PROT_WRITE  (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_PERMISSION | MM_EXACUTE_EL0_PERMISSION | MM_EXACUTE_EL1_PERMISSION)
#define PROT_EXEC   (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_READ_WRITE_PERMISSION | MM_ACCESS_PERMISSION) 


int strcmp(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }

        if (*str1 == '\0' && *str2 != '\0') {
            return 1;
        }
		else if (*str1 != '\0' && *str2 == '\0') {
			return 1;
		}
		else if (*str1 == '\0' && *str2 == '\0') {
			return 0;
		}
        str1++;
        str2++;
    }
    return 1;
}

// This function is required by printf function
void putc ( void* p, char c)
{
	write((unsigned long)&c);
}

void init_print() {
    init_printf(0, putc);
}
