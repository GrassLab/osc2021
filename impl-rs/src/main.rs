#![no_std]
#![no_main]
#![feature(global_asm)]

use core::panic::PanicInfo;

mod cpu;
mod uart;

global_asm!(include_str!("boot.S"));

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
pub unsafe fn main() -> ! {
    uart::init_uart();
    loop {
        let c = uart::getc();
        uart::send(c);
    }
}
