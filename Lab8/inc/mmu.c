#include "mmu.h"
#include "error.h"
#include "allocator.h"
#include "uart.h"

//number of the most significant bits that must be either all 0s or all 1s
#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
//smallest block of memory that can be independently mapped in the translation tables
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

//Peripheral access
#define MAIR_DEVICE_nGnRnE 0b00000000
//Normal RAM access
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1
#define MAIR_CONFIG_DEFAULT ((MAIR_DEVICE_nGnRnE<<(MAIR_IDX_DEVICE_nGnRnE*8))|(MAIR_NORMAL_NOCACHE<<(MAIR_IDX_NORMAL_NOCACHE*8)))

#define PD_TABLE 0b11
#define BOOT_PGD_ATTR PD_TABLE
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)//for L1 (ARM peripherals)
#define BOOT_L2D_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)//for L2 (peripherals)
#define BOOT_L2N_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)//for L2 (normal)

#define VA2PA(x) ((unsigned long)(x)&0xffffffffffff)
#define PA2VA(x) ((unsigned long)(x)|0xffff000000000000)
#define debug 0

void dupPT(void* page_table_src,void* page_table_dst,int level){
	if(page_table_src==0||page_table_dst==0)ERROR("invalid table!");

	//frame
	if(level==4){
		char* src=(char*)PA2VA(page_table_src);
		char* dst=(char*)PA2VA(page_table_dst);
		for(int i=0;i<4096;++i)dst[i]=src[i];
		return;
	}

	//table
	unsigned long* table_src=(unsigned long*)PA2VA(page_table_src);
	unsigned long* table_dst=(unsigned long*)PA2VA(page_table_dst);
	for(int i=0;i<512;++i){
		if(table_src[i]!=0){
			initPT((void**)&table_dst[i]);
			dupPT((void*)(table_src[i]&0xfffffffff000),(void*)(table_dst[i]),level+1);

			unsigned long tmp=table_src[i]&0xfff;
			table_dst[i]|=tmp;
		}
	}
}

void removePT(void* page_table,int level){
	if(page_table==0)ERROR("invalid table!");

	#if debug
		for(int i=0;i<level;++i)uart_send('\t');
		uart_printf("%x\n",page_table);
	#endif

	//frame
	if(level==4){
		ffree(PA2VA(page_table));
		return;
	}

	//table
	unsigned long* table=(unsigned long*)PA2VA(page_table);
	for(int i=0;i<512;++i){
		if(table[i]!=0){
			unsigned long tmp=table[i];
			tmp=tmp-(tmp&0xfff);
			removePT((void*)tmp,level+1);
		}
	}
	ffree(PA2VA(page_table));
}

void initPT(void** page_table){
	char* table=(char*)falloc(4096);
	for(int i=0;i<4096;++i)table[i]=0;
	*page_table=(void*)VA2PA(table);
}

void* updatePT(void* page_table0,void* va){//all address in table are physical address
	if(page_table0==0)ERROR("invalid table!");

	unsigned long tmp=(unsigned long)va;
	int index[4];
	tmp>>=12;
	index[3]=tmp&0x1ff;
	tmp>>=9;
	index[2]=tmp&0x1ff;
	tmp>>=9;
	index[1]=tmp&0x1ff;
	tmp>>=9;
	index[0]=tmp&0x1ff;

	unsigned long* table=(unsigned long*)PA2VA(page_table0);
	for(int i=0;i<=2;++i){
		if(table[index[i]]==0){
			initPT((void**)&table[index[i]]);
			table[index[i]]|=PD_TABLE;
		}
		tmp=table[index[i]];
		tmp=tmp-(tmp&0xfff);
		table=(unsigned long*)PA2VA(tmp);
	}
	if(table[index[3]]!=0)ERROR("invalid va!");
	void* frame=falloc(4096);
	for(int i=0;i<4096;++i)((char*)frame)[i]=0;
	table[index[3]]=VA2PA(frame)|(1<<10)|(1<<6)|MAIR_IDX_NORMAL_NOCACHE<<2|0b11;
	return frame;
}

void initMMULower(){//convenient to debug by aborting lower VA region
	asm volatile("msr ttbr0_el1, %0\n"::"r"(-1));
}

void initMMU(){//be careful when using stack memory!
	//setup tcr & mair
	asm volatile("msr tcr_el1, %0\n"::"r"(TCR_CONFIG_DEFAULT));
	asm volatile("msr mair_el1, %0\n"::"r"(MAIR_CONFIG_DEFAULT));

	//L0 table init
	asm volatile("str %0, [%1]\n"::"r"(0x1000|BOOT_PGD_ATTR),"r"(0));

	//L1 table init
	asm volatile("str %0, [%1]\n"::"r"(0x2000|BOOT_PUD_ATTR),"r"(0x1000));//finer granularity for different memory type
	asm volatile("str %0, [%1]\n"::"r"(0x40000000|BOOT_PUD_ATTR),"r"(0x1000+8));

	//L2 table for 0~1G
	asm volatile("\
		mov x10, %0\n\
		mov x11, %1\n\
		mov x0, #0x2000\n\
		mov x1, #512\n\
		mov x2, #0\n\
beg1:\n\
		cbz x1, end1\n\
		ldr x3, =0x3F000000\n\
		cmp x2, x3\n\
		blt normalmem\n\
peripheralsmem:\n\
		orr x3, x2, x10\n\
		b end2\n\
normalmem:\n\
		orr x3, x2, x11\n\
		b end2\n\
end2:\n\
		str x3, [x0]\n\
		add x0, x0, #8\n\
		sub x1, x1, #1\n\
		add x2, x2, #0x200000\n\
		b beg1\n\
end1:\n\
	"::"r"(BOOT_L2D_ATTR),"r"(BOOT_L2N_ATTR));

	//setting L0 table for lower VA region
	asm volatile("msr ttbr0_el1, %0\n"::"r"(0));//ensure to read correct inst when mmu opened

	//setting L0 table for higher VA region
	asm volatile("msr ttbr1_el1, %0\n"::"r"(0));

	//enalble mmu
	asm volatile("\
		mrs x2, sctlr_el1\n\
		orr x2 , x2, 1\n\
		msr sctlr_el1, x2\n\
	"::);

	//no longer running on 0x80000
	asm volatile("\
		ldr x0, =0xffff000000000000\n\
		add x30, x30, x0\n\
	"::);
}
