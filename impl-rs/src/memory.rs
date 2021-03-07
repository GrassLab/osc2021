use core::{cell::UnsafeCell, ops::RangeInclusive};

// Symbols from the linker script.
extern "Rust" {
    static __bss_start: UnsafeCell<u64>;
    static __bss_end_inclusive: UnsafeCell<u64>;
}

/// The board's memory map.
#[rustfmt::skip]
pub(super) mod map {
    pub const BOOT_CORE_STACK_END: usize = 0x8_0000;

    pub const GPIO_OFFSET:         usize = 0x0020_0000;
    pub const MINIUART_OFFSET:     usize = 0x0021_5000;

    /// Physical devices.
    pub mod mmio {
        use super::*;

        pub const START:            usize =         0x3F00_0000;
        pub const GPIO_START:       usize = START + GPIO_OFFSET;
        pub const MINI_UART_START:  usize = START + MINIUART_OFFSET;
    }
}

pub fn boot_core_stack_end() -> usize {
    map::BOOT_CORE_STACK_END
}

pub fn bss_range_inclusive() -> RangeInclusive<*mut u64> {
    let range;
    unsafe {
        range = RangeInclusive::new(__bss_start.get(), __bss_end_inclusive.get());
    }
    assert!(!range.is_empty());
    range
}
