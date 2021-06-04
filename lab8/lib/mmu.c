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
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)

void initMMULower(){//covenient to debug by aborting lower VA region
	asm volatile("msr ttbr0_el1, %0\n"::"r"(-1));
}

void initMMU(){
	//be careful when using stack memory!

	//setup tcr & mair
	asm volatile("msr tcr_el1, %0\n"::"r"(TCR_CONFIG_DEFAULT));
	asm volatile("msr mair_el1, %0\n"::"r"(MAIR_CONFIG_DEFAULT));
	//L0 table init
	asm volatile("str %0, [%1]\n"::"r"(0x1000|BOOT_PGD_ATTR),"r"(0));
	//L1 table init
	asm volatile("str %0, [%1]\n"::"r"(0x00000000|BOOT_PUD_ATTR),"r"(0x1000));
	asm volatile("str %0, [%1]\n"::"r"(0x40000000|BOOT_PUD_ATTR),"r"(0x1000+8));
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
