TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)g++
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

BUILD_DIR = build
LIB_DIR = libs
SRC_DIR = src
IMG_DIR = img

SRCS = $(wildcard $(SRC_DIR)/*.c)
IMGS = $(SRCS:$(SRC_DIR)/%.c=$(IMG_DIR)/%.img)
SRCS_ASM = $(wildcard $(SRC_DIR)/*.S)
IMGS_ASM = $(SRCS_ASM:$(SRC_DIR)/%.S=$(IMG_DIR)/%.img)

SRCS_LIB = $(wildcard $(LIB_DIR)/*.c)
OBJS_LIB = $(SRCS_LIB:$(LIB_DIR)/%.c=$(BUILD_DIR)/%.o)
SRCS_LIB_ASM = $(wildcard $(LIB_DIR)/*.S)
OBJS_LIB_ASM = $(SRCS_LIB_ASM:$(LIB_DIR)/%.S=$(BUILD_DIR)/%.o)

CFLAGS = -Wall -O1 -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinclude

all: $(IMGS) $(IMGS_ASM)
	cp -v img/* ../rootfs/demo/
	echo "The user program img save under dir:img and dir:../rootfs/demo/"

$(IMG_DIR)/%.img: $(SRC_DIR)/%.c $(IMG_DIR) $(OBJS_LIB) $(BUILD_DIR) $(OBJS_LIB) $(OBJS_LIB_ASM)
	$(CC) $(CFLAGS) -c $< -o out.o
	$(LD) -nostdlib -nostartfiles $(OBJS_LIB) $(OBJS_LIB_ASM) out.o -T linker.ld -o out.elf
	$(OBJCPY) -O binary out.elf $@

$(IMG_DIR)/%.img: $(SRC_DIR)/%.S $(IMG_DIR) $(OBJS_LIB) $(BUILD_DIR) $(OBJS_LIB) $(OBJS_LIB_ASM)
	$(CC) $(CFLAGS) -c $< -o out.o
	$(LD) -nostdlib -nostartfiles $(OBJS_LIB) $(OBJS_LIB_ASM) out.o -T linker.ld -o out.elf
	$(OBJCPY) -O binary out.elf $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.S $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

	


build_dir: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	
lib_dir: $(LIB_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

img_dir: $(IMG_DIR)

$(IMG_DIR):
	mkdir -p $(IMG_DIR)
