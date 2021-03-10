use crate::uart;
use crate::{log, print, println};
use cmd::CmdType;
use core::str;

mod buffer;
mod cmd;

enum Keyboard {
    BackSpace(char),
    NewLine(char),
    Printable(char),
    NotSupported(char),
}

impl Keyboard {
    fn parse(c: char) -> Self {
        match c as u8 {
            8 | 127 => Self::BackSpace(c),
            10 | 13 => Self::NewLine(c),
            32..=126 => Self::Printable(c),
            _ => Self::NotSupported(c),
        }
    }
}

pub struct Shell {
    buffer: buffer::Buffer,
}

impl Shell {
    pub fn new() -> Self {
        return Shell {
            buffer: buffer::Buffer::new(),
        };
    }
    fn log_buffer_status(&self) {
        let s = str::from_utf8(self.buffer.data()).unwrap();
        log!("    buffer :`{}`", s);
    }
    pub fn build_prompt(&self) {
        print!(" {}", ">");
    }
    pub fn input_line(&mut self) {
        self.buffer.clear();
        loop {
            let kb = Keyboard::parse(uart::getc());
            match kb {
                Keyboard::NewLine(_) => {
                    print!("{}", "\n");
                    log!("Press enter!");
                    self.log_buffer_status();
                    break;
                }
                Keyboard::BackSpace(_) => {
                    if let Ok(_) = self.buffer.pop() {
                        print!("\u{08} \u{08}");
                    }
                }
                Keyboard::Printable(c) => {
                    if let Ok(_) = self.buffer.push(c) {
                        print!("{}", c);
                    }
                }
                Keyboard::NotSupported(_) => {}
            }
        }
    }
    pub fn process_command(&mut self) {
        let s = str::from_utf8(self.buffer.data()).unwrap().trim();
        if s.is_empty() {
            return;
        }
        match CmdType::parse(s) {
            CmdType::Cmd(func) => func(),
            CmdType::Unknown => {
                println!(
                    "Unkown command: {:?}, type `help` to get available commands",
                    s
                );
            }
        }
    }
}
