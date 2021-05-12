.section ".text"

.global get_fp
.global get_lr
.global get_sp

get_fp:
  mov x0, fp
  ret

get_lr:
  mov x0, lr
  ret

get_sp:
  mov x0, sp
  ret
