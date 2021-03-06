
TARGET    = aarch64-unknown-none-softfloat
KERNEL_BIN = kernel8.img
LINKER_FILE = src/linker.ld

# Export for build.rs
export LINKER_FILE
KERNEL_ELF = target/$(TARGET)/release/kernel


RUSTFLAGS = -C link-arg=-T$(LINKER_FILE) \
			-C target-cpu=cortex-a53
COMPILER_ARGS = --target=$(TARGET) \
    --release


RUSTC_CMD  = cargo rustc $(COMPILER_ARGS)
OBJCOPY_CMD = rust-objcopy \
    --strip-all            \
    -O binary

all: $(KERNEL_BIN)

$(KERNEL_ELF): FORCE
	RUSTFLAGS="$(RUSTFLAGS)" $(RUSTC_CMD)

$(KERNEL_BIN): $(KERNEL_ELF)
	@$(OBJCOPY_CMD) $(KERNEL_ELF) $(KERNEL_BIN)

clean:
	cargo clean
	$(RM)  kernel8.img

FORCE: