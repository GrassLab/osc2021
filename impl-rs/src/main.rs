#![no_std]
#![no_main]
#![feature(global_asm)]

use core::panic::PanicInfo;

mod config;
mod cpu;
mod print;
mod shell;
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
    use shell::Shell;
    let mut shell = Shell::new();

    loop {
        shell.build_prompt();
        shell.input_line();
        shell.process_command();
    }
}
