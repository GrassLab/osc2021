use core::{cell::UnsafeCell, ops::RangeInclusive};
use cortex_a::asm;
use cortex_a::regs::*;

// Symbols from the linker script.
extern "Rust" {
    static __bss_start: UnsafeCell<u64>;
    static __bss_end_inclusive: UnsafeCell<u64>;
}

#[no_mangle]
pub unsafe fn _start() -> ! {
    const STACK_TOP: usize = 0x8_0000;

    if 0 == core_id() {
        SP.set(STACK_TOP as u64);
        zero_volatile(bss_range_inclusive());
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

/// Pause execution on the core.
#[inline(always)]
pub fn wait_forever() -> ! {
    loop {
        asm::wfe()
    }
}

pub fn bss_range_inclusive() -> RangeInclusive<*mut u64> {
    let range;
    unsafe {
        range = RangeInclusive::new(__bss_start.get(), __bss_end_inclusive.get());
    }
    assert!(!range.is_empty());
    range
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
