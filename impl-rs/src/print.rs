use crate::uart::send;
use core::fmt;
use lazy_static::lazy_static;
use spin::Mutex;

/**
 * The purpose of this mod is to emulate the standard `print!` and `println!` macro for our device(UART).
 * (We wrtie our own `print!` and `println!` and expose them globally).
 * Inspecting those two macros, `format_args!` is the one to format the pass-in string, so it's nice to leverage this macro.
 * However, the return type of `format_args!` is `std::fmt::Arguments`, which need to be supported.
 *
 * In this case, `std::fmt::Write::write_fmt` is designed to handle `std::fmt::Arguments`, we get this method by simply
 * implementing the `std::fmt::Write` Trait, which turns out to be really simple (Defining a `write_str` func is enough)
 */

/**
 * We want to define a global mutable static variable to use, but:
 *
 * 1. Global static is not usable because static obejct is compiled using the [`const evaluator`](https://rustc-dev-guide.rust-lang.org/const-eval.html),
 * it's functionality is still limited(at least in early 2021).
 * 2. The default type of Global static is `const`, in order to use it as a mutable object, we need to wrap it by a Mutex.
 */
lazy_static! {
    static ref WRITER: Mutex<Writer> = Mutex::new(Writer {});
}

pub struct Writer;

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for b in s.bytes() {
            send(b as char);
        }
        Ok(())
    }
}

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => ($crate::print::_print(format_args!($($arg)*)));
}

#[macro_export]
macro_rules! println {
    () => ($crate::print!("\n"));
    ($($arg:tt)*) => ($crate::print!("{}\n", format_args!($($arg)*)));
}

#[doc(hidden)]
pub fn _print(args: fmt::Arguments) {
    use core::fmt::Write;
    WRITER.lock().write_fmt(args).unwrap();
}
