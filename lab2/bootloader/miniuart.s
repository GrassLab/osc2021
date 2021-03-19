.section ".text"

.global miniuart_init
.global miniuart_recv
.global miniuart_recv_u32
.global miniuart_send_u32
.global miniuart_send_S

//250MHz
delay:
  subs w0, w0, #1
  bne delay
  ret

miniuart_init:
  str lr, [sp, #-16]!
  mov x0, #0x00000004
  movk x0, #0x3F20, lsl #16
  ldr w1, [x0]
  and w1, w1, #0xFFFC0FFF
  mov w2, #0x00002000
  movk w2, #0x0001, lsl #16
  orr w1, w1, w2
  str w1, [x0]
  mov x0, #0x00000094
  movk x0, #0x3F20, lsl #16
  ldr w1, [x0]
  and w1, w1, #0xFFFFFFFC
  str w1, [x0]
  mov w0, #150
  bl delay
  mov x19, #0x00000098
  movk x19, #0x3F20, lsl #16
  ldr w1, [x19]
  orr w1, w1, #0x0000C000
  str w1, [x19]
  mov w0, #150
  bl delay
  ldr w1, [x19]
  and w1, w1, #0xFFFF3FFF
  str w1, [x19]
  mov x0, #0x00005004
  movk x0, #0x3F21, lsl #16
  ldr w1, [x0]
  orr w1, w1, #1
  str w1, [x0]
  mov x19, #0x00005060
  movk x19, #0x3F21, lsl #16
  ldr w1, [x19]
  and w1, w1, #0xFFFFFFFC
  str w1, [x19]
  mov x0, #0x00005044
  movk x0, #0x3F21, lsl #16
  ldr w1, [x0]
  and w1, w1, #0xFFFFFFFC
  str w1, [x0]
  mov x0, #0x0000504C
  movk x0, #0x3F21, lsl #16
  ldr w1, [x0]
  orr w1, w1, #0b11
  str w1, [x0]
  mov x0, #0x00005050
  movk x0, #0x3F21, lsl #16
  ldr w1, [x0]
  and w1, w1, #0xFFFFFFFD
  str w1, [x0]
  mov x0, #0x00005068
  movk x0, #0x3F21, lsl #16
  ldr w1, [x0]
  and w1, w1, #0xFFFF0000
  mov w2, #270
  orr w1, w1, w2
  str w1, [x0]
  mov x0, #0x00005048
  movk x0, #0x3F21, lsl #16
  ldr w1, [x0]
  orr w1, w1, #0b110
  str w1, [x0]
  ldr w1, [x19]
  orr w1, w1, #0b11
  str w1, [x19]
  ldr lr, [sp], #16
  ret

miniuart_recv:
  mov x0, #0x00005054
  movk x0, #0x3F21, lsl #16
  mov x1, #0x00005040
  movk x1, #0x3F21, lsl #16
data_not_ready:
  ldr w2, [x0]
  ands w2, w2, #1
  beq data_not_ready
  ldr w0, [x1]
  and w0, w0, 0xFF
  ret

miniuart_recv_u32:
  str lr, [sp, #-16]!
  mov w3, #0
  mov w4, #0
1:
  bl miniuart_recv
  lsl w1, w4, #3
  lsl w0, w0, w1
  orr w3, w3, w0
  add w4, w4, #1
  cmp w4, #4
  bne 1b
  mov w0, w3
  ldr lr, [sp], #16
  ret

miniuart_send_C:
  mov x1, #0x00005054
  movk x1, #0x3F21, lsl #16
  mov x2, #0x00005040
  movk x2, #0x3F21, lsl #16
data_not_empty:
  ldr w3, [x1]
  ands w3, w3, #0x20
  beq data_not_empty
  str w0, [x2]
  ret

miniuart_send_S:
  str lr, [sp, #-16]!
  mov x4, x0
  ldrb w0, [x4]
1:
  bl miniuart_send_C
  add x4, x4, #1
  ldrb w0, [x4]
  cmp w0, #0
  bne 1b
  ldr lr, [sp], #16
  ret

miniuart_send_u32:
  str lr, [sp, #-16]!
  mov w4, w0
  mov w5, #4
1:
  and w0, w4, 0xFF
  bl miniuart_send_C
  lsr w4, w4, #8
  subs w5, w5, #1
  bne 1b
  ldr lr, [sp], #16
  ret
