cycles .req x0

.global delay

delay:
    tbz cycles, #0, 1f
    sub cycles, cycles, #1
1:
    cbz cycles, 3f
2:
    sub cycles, cycles, #2
    cbnz cycles, 2b
3:
    ret

