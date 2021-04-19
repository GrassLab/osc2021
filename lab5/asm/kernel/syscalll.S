/**
 * syscall_x0(x1, x2...x8)
 */

.section ".text"
.global syscall

syscall:
  ldr x9, =syscall_table
  lsl x0, x0, #4
  add x0, x0, x9
  #smaller than syscall_table
  cmp x9, x0
  bgt fail 
  ldr x10, =syscall_table_end
  #larger than syscall_table
  cmp x0, x10
  bge fail
  br x0
fail:
  b syscall_number_error

.align 4
.global syscall_table
.global syscall_table_end
syscall_table:
  #getpid
  b sys_getpid
  .align 4
  /*#uart_read
  b sys_uart_read
  .align 4
  #uart_write
  /*b sys_uart_write
  .align 4*/
  #exec
  b sys_exec
  .align 4
  #exit
  b sys_exit
  .align 4
  #fork
  b sys_fork
  .align 4
syscall_table_end:
