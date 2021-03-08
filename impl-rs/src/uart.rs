use crate::{cpu, gpio, memory};
use register::{mmio::*, register_bitfields, register_structs};

register_bitfields! {
    u32,
    AUXENB [
        MINI_UART OFFSET(0) NUMBITS(1) [
            On = 0b1,
            Off = 0b0
        ]
    ],

    AUX_MU_IO [
        DATA OFFSET(0) NUMBITS(8)[]
    ],

    AUX_MU_IER[
        ENB_RECV_INT OFFSET(0) NUMBITS(1) []
    ],

    AUX_MU_IIR[
        FIFO_ENB OFFSET(6) NUMBITS(2)[
            Off = 0b11
        ]
    ],

    AUX_MU_LCR[
        /// Data size. These bits control the line data format
        DSIZE OFFSET(0) NUMBITS(2)[
            EightBit = 0b11,
            SevenBit = 0b00
        ]
    ],

    AUX_MU_MCR[
        RTS OFFSET(1) NUMBITS(1)[
            High = 0b0,
            Low = 0b1
        ]
    ],

    AUX_MU_LSR[
        TX_EMPTY OFFSET(5) NUMBITS(1)[
            True = 0b1,
            False = 0b0
        ],
        // Receive FIFO hold at least 1 symbol
        RX_RDY OFFSET(0) NUMBITS(1)[
            True = 0b1,
            False = 0b0
        ]
    ],
    AUX_MU_CNTL[
        RX_ENB OFFSET(0) NUMBITS(1)[
            On = 0b1,
            Off = 0b0
        ],
        TX_ENB OFFSET(1) NUMBITS(1)[
            On = 0b1,
            Off = 0b0
        ]
    ],
    AUX_MU_BAUD[
        BAUD OFFSET(0) NUMBITS(16)[
            Rate115200 = 270
        ]
    ]
}

register_structs! {
    #[allow(non_snake_case)]
    pub RegisterBlock {
        (0x04 => AUXENB:ReadWrite<u32, AUXENB::Register>),
        (0x08 => _reserved1),
        (0x40 => AUX_MU_IO:ReadWrite<u32, AUX_MU_IO::Register>),
        (0x44 => AUX_MU_IER:ReadWrite<u32, AUX_MU_IER::Register>),
        (0x48 => AUX_MU_IIR:ReadWrite<u32, AUX_MU_IIR::Register>),
        (0x4C => AUX_MU_LCR:ReadWrite<u32, AUX_MU_LCR::Register>),
        (0x50 => AUX_MU_MCR:ReadWrite<u32, AUX_MU_MCR::Register>),
        (0x54 => AUX_MU_LSR:ReadWrite<u32, AUX_MU_LSR::Register>),
        (0x58 => _reserved2),
        (0x60 => AUX_MU_CNTL:ReadWrite<u32, AUX_MU_CNTL::Register>),
        (0x64 => _reserved3),
        (0x68 => AUX_MU_BAUD:ReadWrite<u32, AUX_MU_BAUD::Register>),
        (0x6C => @END),
    }
}

pub fn init_uart() {
    let regs = memory::map::mmio::MINI_UART_START as *const RegisterBlock;
    unsafe {
        (*regs).AUXENB.modify(AUXENB::MINI_UART::On);
        (*regs)
            .AUX_MU_CNTL
            .modify(AUX_MU_CNTL::RX_ENB::Off + AUX_MU_CNTL::TX_ENB::Off);
        (*regs).AUX_MU_LCR.modify(AUX_MU_LCR::DSIZE::EightBit);
        (*regs).AUX_MU_MCR.modify(AUX_MU_MCR::RTS::High);
        (*regs).AUX_MU_IER.set(0);
        (*regs).AUX_MU_IIR.modify(AUX_MU_IIR::FIFO_ENB::Off);
        (*regs).AUX_MU_BAUD.modify(AUX_MU_BAUD::BAUD::Rate115200);
        gpio::map_mini_uart();
        (*regs)
            .AUX_MU_CNTL
            .modify(AUX_MU_CNTL::RX_ENB::On + AUX_MU_CNTL::TX_ENB::On);
    }
}
pub fn read_char() -> char {
    let regs = memory::map::mmio::MINI_UART_START as *const RegisterBlock;
    unsafe {
        while (*regs).AUX_MU_LSR.matches_all(AUX_MU_LSR::RX_RDY::False) {
            cpu::nop();
        }
        let c: char = (*regs).AUX_MU_IO.get() as u8 as char;
        if c == '\r' {
            return '\n';
        }
        return c;
    }
}

pub fn write_char(c: char) {
    let regs = memory::map::mmio::MINI_UART_START as *const RegisterBlock;
    unsafe {
        while (*regs).AUX_MU_LSR.matches_all(AUX_MU_LSR::TX_EMPTY::False) {
            cpu::nop();
        }
        (*regs).AUX_MU_IO.set(c as u32);
    }
}

// pub fn write_str(s: &str) {
//     for c in s.chars() {
//         write_char(c);
//     }
// }
