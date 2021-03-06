#![no_std]
#![no_main]

use core::panic::PanicInfo;
mod boot;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

pub unsafe fn main() -> ! {
    loop {}
}
