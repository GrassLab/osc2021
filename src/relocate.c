extern unsigned long long _start, _end, _reloc_addr;

void relocate() {
  unsigned long size = (&_end - &_start);
  unsigned long long *target = &_reloc_addr;
  unsigned long long *origin = &_start;
  while(size--) {
    *target++ = *origin++;
  }
}