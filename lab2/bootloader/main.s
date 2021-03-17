.section .rodata
  welcome_str: .string "Mini UART bootloader\r\nPress Q to start\r\n"

.section ".text.startup"

_start:
  mov sp, #0x00800000
  mov x22, x0
  bl miniuart_init

uboot:
  adr x0, welcome_str
  bl miniuart_send_S
wait_Q:
  bl miniuart_recv
  cmp w0, #81
  bne wait_Q
  bl miniuart_recv_u32

copy_loader:
  mov x19, x0
  mov x20, x0
  mov x1, #0b111
  add x19, x19, x1
  mvn x1, x1
  and x19, x19, x1
  add x19, x19, #0x00080000
  mov x2, x19
  adr x3, load_kernel
  adr x4, load_kernel_end
  subs x4, x4, x3
1:
  ldr x1, [x3], #8
  str x1, [x2], #8
  subs x4, x4, #8
  bne 1b
  bl miniuart_send_u32
  mov x0, x20
  br x19

  .align(3)
load_kernel:
  mov x1, #0x00080000
  mov x2, #0x00005054
  movk x2, #0x3F21, lsl #16
  mov x3, #0x00005040
  movk x3, #0x3F21, lsl #16
data_not_ready:
  ldr w4, [x2]
  ands w4, w4, #1
  beq data_not_ready
  ldr w4, [x3]
  and w4, w4, 0xFF
  strb w4, [x1]
  add x1, x1, #1
  subs x0, x0, #1
  bne data_not_ready
  mov x0, x22
  mov x1, #0x00080000
  br x1
  .align(3)
load_kernel_end:
