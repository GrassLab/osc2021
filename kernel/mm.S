#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)
#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1
#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define BOOT_L2_DEVICE_ATTR  (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define BOOT_L2_NORMAL_ATTR  (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)
#define PAGE_SIZE (1 << 12)

.global memcpy
memcpy:
	ldr x3, [x1], #8
	str x3, [x0], #8
	subs x2, x2, #8
	b.gt memcpy
	ret

.global memzero
memzero:
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret

.global vm_init
vm_init:
	/* required 1-1 */
	ldr x0, = TCR_CONFIG_DEFAULT
	msr tcr_el1, x0

	/* required 1-2 */
	ldr x0, =( \
		(MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
		(MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
	)
	msr mair_el1, x0

	/* required 1-3: setup PGD */
	mov x0, #0x0000 // PGD's page frame at 0x0
	mov x1, #0x1000 // PUD's page frame at 0x1000

	ldr x2, = BOOT_PGD_ATTR
	orr x2, x1, x2 // combine the physical address of next level page with attribute.
	str x2, [x0]

	/* required 1-3: setup PUD */
	mov x0, #0x1000 // PUD's page frame start from 0x1000
	mov x1, #0x2000 // PMD's page frame start from 0x2000
	ldr x2, = BOOT_PGD_ATTR
	orr x1, x1, x2
	str x1, [x0] // mapped the 1st entry of PUD to PMD
	mov x3, #0x40000000
	ldr x2, = BOOT_PUD_ATTR
	orr x3, x3, x2
	str x3, [x0, 8] // mapped the 2nd entry of PUD to ARM local peripherals

	/* required 1-5: setup PMD */
	mov x10, BOOT_L2_DEVICE_ATTR
	mov x11, BOOT_L2_NORMAL_ATTR
	mov x0, #0x2000 // PMD's page frame statr from 0x2000
	mov x1, #512 // a PMD has 512 items
	mov x2, #0

1:
	cbz x1, openMMU
	ldr x3, = 0x3f000000 // starting address of MMIO
	cmp x2, x3
	blt RAMregion // if current address less than MMIO starting address, indicates that we are mapping RAM to normal memory

MMIOregion:
	orr x3, x2, x10
	b 2f

RAMregion:
	orr x3, x2, x11
	b 2f

2:
	str x3, [x0]
	add x0, x0, #8
	sub x1, x1, #1
	add x2, x2, #0x200000 // 2MB
	b 1b

openMMU:
	/* required 1-4 */
	mov x0, xzr // address of PGD of kernel is 0x0
	msr ttbr0_el1, x0 // load PGD to the bottom translation based register.
	msr ttbr1_el1, x0 // also load PGD to the upper translation based register.
	
	ldr x0, = 0xffff000000000000
	add x30, x30, x0

	/* enable MMU */
	mrs x2, sctlr_el1
	orr x2 , x2, 1
	msr sctlr_el1, x2 // enable MMU, cache remains disabled
	
	ret
	

	