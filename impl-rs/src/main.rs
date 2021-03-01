#![no_std]
#![no_main]
use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

// Replacing program's very entrypoint (C runtime zero)
// by defining our own _start
// Explain: _start symbol is the default entry point name for most systems.
#[no_mangle]
pub extern "C" fn _start() -> ! {
    loop {}
}
