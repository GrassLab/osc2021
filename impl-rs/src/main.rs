#![no_std]
#![no_main]
#![feature(global_asm)]

use core::panic::PanicInfo;

mod cpu;
mod print;
mod uart;

global_asm!(include_str!("boot.S"));

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
pub unsafe fn main() -> ! {
    uart::init_uart();
    println!("-------------------------------");
    println!(" Operating System Capstone 2021");
    println!("-------------------------------");
    loop {
        let c = uart::getc();
        uart::send(c);
    }
}
