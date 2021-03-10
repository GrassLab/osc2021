use crate::cpu;
use core::ptr::{read_volatile, write_volatile};

const MMIO_BASE: u32 = 0x3F00_0000;

const AUX_ENABLE: *mut u32 = (MMIO_BASE + 0x0021_5004) as *mut u32;
const AUX_MU_IO: *mut u32 = (MMIO_BASE + 0x0021_5040) as *mut u32;
const AUX_MU_IER: *mut u32 = (MMIO_BASE + 0x0021_5044) as *mut u32;
const AUX_MU_IIR: *mut u32 = (MMIO_BASE + 0x0021_5048) as *mut u32;
const AUX_MU_LCR: *mut u32 = (MMIO_BASE + 0x0021_504C) as *mut u32;
const AUX_MU_MCR: *mut u32 = (MMIO_BASE + 0x0021_5050) as *mut u32;
const AUX_MU_LSR: *mut u32 = (MMIO_BASE + 0x0021_5054) as *mut u32;
const AUX_MU_CNTL: *mut u32 = (MMIO_BASE + 0x0021_5060) as *mut u32;
const AUX_MU_BAUD: *mut u32 = (MMIO_BASE + 0x0021_5068) as *mut u32;

const GPFSEL1: *mut u32 = (MMIO_BASE + 0x00200004) as *mut u32;
const GPPUD: *mut u32 = (MMIO_BASE + 0x00200094) as *mut u32;
const GPPUDCLK0: *mut u32 = (MMIO_BASE + 0x00200098) as *mut u32;

pub fn init_uart() {
    unsafe {
        write_volatile(AUX_ENABLE, read_volatile(AUX_ENABLE) | 1);
        write_volatile(AUX_MU_CNTL, 0);
        write_volatile(AUX_MU_LCR, 3);
        write_volatile(AUX_MU_MCR, 0);
        write_volatile(AUX_MU_IER, 0);
        write_volatile(AUX_MU_IIR, 0xc6);
        write_volatile(AUX_MU_BAUD, 270);
        /* map UART1 to GPIO pins */
        let mut r = read_volatile(GPFSEL1);
        r &= !((7 << 12) | (7 << 15));
        r |= (2 << 12) | (2 << 15);
        write_volatile(GPFSEL1, r);
        write_volatile(GPPUD, 0);
        cpu::spin_for_cycles(150);
        write_volatile(GPPUDCLK0, (1 << 14) | (1 << 15));
        cpu::spin_for_cycles(150);
        write_volatile(GPPUDCLK0, 0);
        write_volatile(AUX_MU_CNTL, 3);
    }
}

pub fn getc() -> char {
    unsafe {
        while 0 == (read_volatile(AUX_MU_LSR) & 0x01) {
            cpu::nop();
        }
        let c = read_volatile(AUX_MU_IO) as u8 as char;
        if c == '\r' {
            return '\n';
        }
        return c;
    }
}

pub fn send(c: char) {
    unsafe {
        while 0 == (read_volatile(AUX_MU_LSR) & 0x20) {
            cpu::nop();
        }
        write_volatile(AUX_MU_IO, c as u32);
    }
}
