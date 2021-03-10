
TARGET    = aarch64-unknown-none-softfloat
KERNEL_BIN = kernel8.img
LINKER_FILE = src/linker.ld
DEBUG = 1

ifeq ($(DEBUG), 1)
    BUILD_TYPE = debug
else
    BUILD_TYPE = release
endif
KERNEL_ELF = target/$(TARGET)/$(BUILD_TYPE)/kernel

# Export for build.rs
export LINKER_FILE
RUSTFLAGS = -C link-arg=-T$(LINKER_FILE) \
			-C target-cpu=cortex-a53

COMPILER_ARGS = --target=$(TARGET)
ifneq ($(DEBUG), 1)
    COMPILER_ARGS += --release
endif


RUSTC_CMD  = cargo rustc $(COMPILER_ARGS)
OBJCOPY_CMD = rust-objcopy \
    -O binary
ifneq ($(DEBUG), 1)
    OBJCOPY_CMD += --strip-all
endif

all: $(KERNEL_BIN)

$(KERNEL_ELF): FORCE
	RUSTFLAGS="$(RUSTFLAGS)" $(RUSTC_CMD)

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY_CMD) $(KERNEL_ELF) $(KERNEL_BIN)

clean:
	cargo clean
	$(RM)  kernel8.img

FORCE: