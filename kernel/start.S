
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

.section ".text.boot"
.global _start
.global loop
.global reset
_start:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    mov x1, #1
    cmp x0, x1
    b.eq .Lel1
    mov x0, (1 << 31)
    msr hcr_el2, x0
    mov x0, 0x3c5
    msr spsr_el2, x0
    adr x0, .Lel1
    movk x0, 0xffff, lsl #48
    msr elr_el2, x0
    mov x0, #1
    msr cntp_ctl_el0, x0
    mrs x1, cpacr_el1
    mov x0, #(3 << 20)
    orr x0, x1, x0
    msr cpacr_el1, x0
    adr x5, exception_vector_table
    mov x6, #0x3c0
    msr vbar_el1, x5
    msr spsr_el1, x6
    msr tpidr_el1, xzr
// TLB
    ldr x0, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x0
    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    mov x0, 0 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000

    ldr x2, = BOOT_PGD_ATTR
    orr x2, x1, x2 // combine the physical address of next level page with attribute.
    str x2, [x0]

    ldr x2, = BOOT_PUD_ATTR
    mov x3, 0x00000000
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    mov x3, 0x40000000
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    msr ttbr0_el1, x0 // load PGD to the bottom translation based register.
    msr ttbr1_el1, x0

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled.

    eret
.Lel1:
    adr x4, _start
    adr x0, __bss_start__
    adr x3, __bss_end__
    mov x1, xzr
    mov sp, x4
    sub x2, x3, x0
    bl memset
    bl kmain
loop:
    wfe
    b loop
reset:
    mov x0, 0x20
    movk x0, 0x5a00, lsl #16
    mov x1, 0x1c
    movk x1, 0x3f10, lsl #16
    str w0, [x1]
    movk x1, 0x24, lsl #0
    str w0, [x1]
    b loop
