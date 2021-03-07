pub mod interface {
    use core::fmt;

    /// Console write functions.
    pub trait Write {
        /// Write a single character.
        fn write_char(&mut self, c: char);

        /// Write a Rust format string.
        fn write_fmt(&mut self, args: fmt::Arguments) -> fmt::Result;
    }

    /// Console read functions.
    pub trait Read {
        /// Read a single character.
        fn read_char(&self) -> char {
            ' '
        }
    }
    /// Trait alias for a full-fledged console.
    pub trait All = Write + Read;
}
