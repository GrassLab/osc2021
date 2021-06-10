.section ".text.startup"

.global vm_init
.global set_ttbr0_el1

.equ TCR_CONFIG_REGION_48bit, (((64 - 48) << 0) | ((64 - 48) << 16))
.equ TCR_CONFIG_4KB, ((0b00 << 14) |  (0b10 << 30))
.equ TCR_CONFIG_DEFAULT, (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

.equ MAIR_DEVICE_nGnRnE, 0b00000000
.equ MAIR_NORMAL_NOCACHE, 0b01000100
.equ MAIR_IDX_DEVICE_nGnRnE, 0
.equ MAIR_IDX_NORMAL_NOCACHE, 1

.equ PD_TABLE, 0b11
.equ PD_BLOCK, 0b01
.equ PD_ACCESS, (1 << 10)
.equ BOOT_PGD_ATTR, PD_TABLE
.equ BOOT_PUD_ATTR, PD_TABLE

.equ BOOT_PMD_DEV_ATTR, (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
.equ BOOT_PMD_RAM_ATTR, (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_BLOCK)

vm_init:
  ldr x0, =TCR_CONFIG_DEFAULT
  msr tcr_el1, x0

  ldr x0, =((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))
  msr mair_el1, x0

  mov x0, 0 // PGD's page frame at 0x0
  mov x1, 0x1000 // PUD's page frame at 0x1000

  ldr x2, =BOOT_PGD_ATTR
  orr x2, x1, x2 // combine the physical address of next level page with attribute.
  str x2, [x0]

  ldr x2, =BOOT_PUD_ATTR
  mov x3, 0x2000 // PMD's page frame #1 at 0x2000
  orr x2, x3, x2
  str x2, [x1]

  ldr x2, =BOOT_PUD_ATTR
  mov x3, 0x3000 // PMD's page frame #2 at 0x3000
  orr x2, x3, x2
  str x2, [x1, 8]

  // RAM -> 0x00000000 ~ 0x3F000000
  ldr x2, =BOOT_PMD_RAM_ATTR
  mov x4, 0x00000000
  mov x4, 0x3F000000
  mov x3, 0x2000
1:
  str x2, [x3], #8
  add x2, x2, (1 << 21) // 2MB
  subs x4, x4, (1 << 21) // 2MB
  bne 1b

  // DEV -> 0x3F000000 ~ 0x80000000
  ldr x2, =BOOT_PMD_DEV_ATTR
  mov x4, 0x3F000000
  orr x2, x4, x2
  mov x5, 0x80000000
  sub x4, x5, x4
1:
  str x2, [x3], #8
  add x2, x2, (1 << 21) // 2MB
  subs x4, x4, (1 << 21) // 2MB
  bne 1b
pause:
  msr ttbr0_el1, x0 // load PGD to the bottom translation based register.
  msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

  mrs x2, sctlr_el1
  orr x2, x2, 1
  msr sctlr_el1, x2 // enable MMU, cache remains disabled

  ret

set_ttbr0_el1:
  dsb ish // ensure write has completed
  msr ttbr0_el1, x0 // switch translation based address.
  tlbi vmalle1is // invalidate all TLB entries
  dsb ish // ensure completion of TLB invalidatation
  isb // clear pipeline
  ret
