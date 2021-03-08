#![no_std]
#![no_main]
#![feature(const_fn_fn_ptr_basics)]
#![feature(format_args_nl)]
#![feature(trait_alias)]

use core::panic::PanicInfo;

mod boot;
mod cpu;
mod gpio;
mod memory;
mod uart;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

pub unsafe fn main() -> ! {
    uart::init_uart();
    loop {
        let c = uart::read_char();
        uart::write_char(c);
    }
}
