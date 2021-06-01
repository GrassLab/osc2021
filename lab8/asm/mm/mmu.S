#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_PAGE 0b11
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR PD_TABLE

#define PD_RAM_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)
#define PD_PERIPHERAL_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)


.global mmu_enable
mmu_enable:
  #translation control register
  ldr x0, = TCR_CONFIG_DEFAULT
  msr tcr_el1, x0

  #memory attribute indirection register
  ldr x0, =( \
    (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
    (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
  )
  msr mair_el1, x0

  #PGD
  mov x0, 0 // PGD's page frame at 0x0
  mov x1, 0x1000 // PUD's page frame at 0x1000
  ldr x2, = BOOT_PGD_ATTR
  orr x2, x1, x2 // combine the physical address of next level page with attribute.
  str x2, [x0]

  #PUD
  ldr x2, =BOOT_PUD_ATTR
  mov x3, 0x2000 // PMD page frame at 0x2000
  orr x3, x2, x3
  str x3, [x1]
  
  #ARM local peripheral
  ldr x2, =PD_PERIPHERAL_ATTR
  mov x3, 0x00000000
  orr x3, x2, x3
  str x3, [x1, 8]

  #PMD
  #map to kernel
  mov x1, 0x2000
  ldr x2, =PD_RAM_ATTR
  mov x3, 0x00000000
  orr x3, x2, x3
  str x3, [x1]

  #map to peripheral
  ldr x2, =PD_PERIPHERAL_ATTR
  
  # GPU peripheral

  # 0x3f000000 - 0x3f1fffff
  mov x3, 0x3f000000
  orr x3, x2, x3
  str x3, [x1, 0x1f8*8]
  
  # 0x3f200000 - 0x3f3fffff
  mov x3, 0x3f200000
  orr x3, x2, x3
  str x3, [x1, 0x1f9*8]
  
  #cpio address

  # 0x08000000 - 0x081fffff
  mov x3, 0x08000000
  orr x3, x2, x3
  str x3, [x1, 0x40*8]
  
  # buddy address
  
  # 0x10000000 - 0x101fffff
  mov x3, 0x10000000
  orr x3, x2, x3
  str x3, [x1, 0x80*8]

  msr ttbr0_el1, x0 // load PGD to the bottom translation based register.
  msr ttbr1_el1, x0
  
  mrs x2, sctlr_el1
  orr x2 , x2, 1
  msr sctlr_el1, x2 // enable MMU, cache remains disabled
  
  #ret
  ldr x2, =boot_rest
  br x2
