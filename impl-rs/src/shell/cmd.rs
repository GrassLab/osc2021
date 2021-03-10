use crate::println;
use core::ptr::write_volatile;

pub enum CmdType {
    Cmd(fn() -> ()),
    Unknown,
}

impl CmdType {
    pub fn parse(s: &str) -> Self {
        match s {
            "hello" => Self::Cmd(exec_hello),
            "help" => Self::Cmd(exec_help),
            "reboot" => Self::Cmd(exec_reboot),
            _ => Self::Unknown,
        }
    }
}

fn exec_hello() {
    println!("Hello !!!");
}

const USAGE: &'static str = "
Available commands
    help        get help message
    hello       say Hi
    reboot      reboot device
";

fn exec_help() {
    println!("{}", USAGE);
}

fn exec_reboot() {
    println!("Start reboot");
    const PM_PASSWORD: u32 = 0x5a000000 as u32;
    const PM_RSTC: *mut u32 = 0x3F10001c as *mut u32;
    const PM_WDOG: *mut u32 = 0x3F100024 as *mut u32;
    unsafe {
        write_volatile(PM_RSTC, PM_PASSWORD | (0x20 as u32));
        write_volatile(PM_WDOG, PM_PASSWORD | (100 as u32));
    }
}
