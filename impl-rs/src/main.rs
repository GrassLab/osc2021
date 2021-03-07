#![no_std]
#![no_main]
#![feature(const_fn_fn_ptr_basics)]
#![feature(format_args_nl)]
#![feature(trait_alias)]

use bsp::{gpio, uart};
use core::panic::PanicInfo;

mod boot;
mod bsp;
mod console;
mod cpu;
mod memory;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

pub unsafe fn main() -> ! {
    let mut gpio = gpio::GPIO::new(memory::map::mmio::GPIO_START);
    gpio.map_mini_uart();
    let mut uart = uart::MINIUART::new(memory::map::mmio::MINI_UART_START);
    uart.init();
    loop {
        uart.write_str("Hello world\n");
        cpu::spin_for_cycles(200);
    }
}
