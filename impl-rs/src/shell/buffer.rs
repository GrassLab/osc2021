use crate::config::MX_BFR_SIZE;

pub struct BfrFullErr;
pub struct BfrEmptyErr;

pub struct Buffer {
    buffer: [u8; MX_BFR_SIZE],
    write_head: usize,
}

impl Buffer {
    pub fn new() -> Self {
        return Buffer {
            buffer: [0 as u8; MX_BFR_SIZE],
            write_head: 0,
        };
    }
    pub fn clear(&mut self) {
        self.write_head = 0;
    }
    pub fn push(&mut self, c: char) -> Result<(), BfrFullErr> {
        if self.write_head < MX_BFR_SIZE {
            self.buffer[self.write_head] = c as u8;
            self.write_head += 1;
            Ok(())
        } else {
            Err(BfrFullErr)
        }
    }
    pub fn pop(&mut self) -> Result<(), BfrEmptyErr> {
        if self.write_head > 0 {
            self.write_head -= 1;
            self.buffer[self.write_head] = '\0' as u8;
            Ok(())
        } else {
            Err(BfrEmptyErr)
        }
    }
    pub fn data(&self) -> &[u8] {
        return &self.buffer[0..self.write_head];
    }
}
