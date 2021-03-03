.globl clearmem
clearmem:
  str xzr, [x0], #8
  subs x1, x1, #8
  bgt clearmem
  ret

.globl clearmembyte
clearmembyte:
  strb wzr, [x0], #1
  subs x1, x1, #1
  bgt clearmembyte
  ret