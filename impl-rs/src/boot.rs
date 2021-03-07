use crate::cpu::wait_forever;
use crate::memory;
use core::ops::RangeInclusive;
use cortex_a::regs::*;

#[no_mangle]
pub unsafe fn _start() -> ! {
    if 0 == core_id() {
        SP.set(memory::boot_core_stack_end() as u64);
        zero_volatile(memory::bss_range_inclusive());
        crate::main();
    } else {
        wait_forever()
    }
}

/// Return the executing core's id.
#[inline(always)]
pub fn core_id<T>() -> T
where
    T: From<u8>,
{
    const CORE_MASK: u64 = 0b11;
    T::from((MPIDR_EL1.get() & CORE_MASK) as u8)
}

unsafe fn zero_volatile<T>(range: RangeInclusive<*mut T>)
where
    T: From<u8>,
{
    let mut ptr = *range.start();
    let end_inclusive = *range.end();

    while ptr <= end_inclusive {
        core::ptr::write_volatile(ptr, T::from(0));
        ptr = ptr.offset(1);
    }
}
