/**
 * syscall_x8(x0, x1...x7)
 */

.section ".text"
.global syscall

syscall:
  ldr x9, =syscall_table
  lsl x8, x8, #4
  add x8, x8, x9
  #smaller than syscall_table
  cmp x9, x8
  bgt fail 
  ldr x10, =syscall_table_end
  #larger than syscall_table
  cmp x8, x10
  bge fail
  br x8
fail:
  b syscall_number_error

.align 4
.global syscall_table
.global syscall_table_end
syscall_table:
  #getpid
  b sys_getpid
  .align 4
  #uart_read
  b sys_uart_read
  .align 4
  #uart_write
  b sys_uart_write
  .align 4
  #exec
  b sys_exec
  .align 4
  #exit
  b sys_exit
  .align 4
  #fork
  b sys_fork
  .align 4
  #get_time
  b sys_get_time
  .align 4
  #open
  b sys_open
  .align 4
  #close
  b sys_close
  .align 4
  #write
  b sys_write
  .align 4
  #read
  b sys_read
  .align 4
syscall_table_end:

.global exit
exit:
  mov x8, #4
  svc #0
  ret