use crate::memory;
use register::{mmio::*, register_bitfields, register_structs};

// Raspberrypi3 Model B+ -> BCM2837B0

// GPIO registers.
//
// Descriptions taken from
// - https://github.com/raspberrypi/documentation/files/1888662/BCM2837-ARM-Peripherals.-.Revised.-.V2-1.pdf
// - https://github.com/raspberrypi/documentation/issues/325
register_bitfields! {
    u32,

    /// GPIO Function Select 1
    GPFSEL1 [
        /// Pin 15
        FSEL15 OFFSET(15) NUMBITS(3) [
            Input  = 0b000,
            Output = 0b001,
            AltFunc5 = 0b010  // Mini UART RX

        ],

        /// Pin 14
        FSEL14 OFFSET(12) NUMBITS(3) [
            Input = 0b000,
            Output = 0b001,
            AltFunc5 = 0b010  // Mini UART TX
        ]
    ],

    /// GPIO Pull-up/down Register
    ///
    /// BCM2837 only.
    GPPUD [
        /// Controls the actuation of the internal pull-up/down control line to ALL the GPIO pins.
        PUD OFFSET(0) NUMBITS(2) [
            Off = 0b00,
            PullDown = 0b01,
            PullUp = 0b10
        ]
    ],

    /// GPIO Pull-up/down Clock Register 0
    GPPUDCLK0 [
        /// Pin 15
        PUDCLK15 OFFSET(15) NUMBITS(1) [
            NoEffect = 0b0,
            AssertClock = 0b1
        ],

        /// Pin 14
        PUDCLK14 OFFSET(14) NUMBITS(1) [
            NoEffect = 0b0,
            AssertClock = 0b1
        ]
    ]

}

register_structs! {
    #[allow(non_snake_case)]
    RegisterBlock {
        (0x04 => GPFSEL1: ReadWrite<u32, GPFSEL1::Register>),
        (0x08 => _reserved1),
        (0x94 => GPPUD: ReadWrite<u32, GPPUD::Register>),
        (0x98 => GPPUDCLK0: ReadWrite<u32, GPPUDCLK0::Register>),
        (0xA0 => @END),
    }
}

pub fn map_mini_uart() {
    let regs = memory::map::mmio::GPIO_START as *const RegisterBlock;
    const DELAY: usize = 200;
    use crate::cpu;

    unsafe {
        (*regs)
            .GPFSEL1
            .modify(GPFSEL1::FSEL15::AltFunc5 + GPFSEL1::FSEL14::AltFunc5);
        (*regs).GPPUD.modify(GPPUD::PUD::Off);
        cpu::spin_for_cycles(DELAY);
        (*regs)
            .GPPUDCLK0
            .write(GPPUDCLK0::PUDCLK15::AssertClock + GPPUDCLK0::PUDCLK14::AssertClock);
        cpu::spin_for_cycles(DELAY);
        (*regs).GPPUDCLK0.set(0);
    }
}
