# OSC 2021, check https://github.com/GrassLab/osc2021
KERNEL_ROOT := $(dir $(lastword $(MAKEFILE_LIST)))
include $(KERNEL_ROOT)../Makefile.inc

SRC_PATH    = $(KERNEL_ROOT)src
LIB_PATH    = $(KERNEL_ROOT)lib
KERNEL_SRC  = $(wildcard $(SRC_PATH)/*.c)
KERNEL_OBJ  = $(patsubst $(SRC_PATH)/%, $(LIB_PATH)/%, ${KERNEL_SRC:.c=.o})
#KERNEL_OBJ  = $(patsubst $(SRC_PATH)/%, $(LIB_PATH)/%, ${$(wildcard $($(SRC_PATH)/*.c)):.c=.o})

CFLAGS      += -I $(KERNEL_ROOT)include

.PHONY: kerobj kercln

kerobj: $(KERNEL_OBJ)
$(LIB_PATH)/%.o: $(SRC_PATH)/%.c
# Compile from C to obj file
	@mkdir -p $(LIB_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

kercln:
	rm -rf $(LIB_PATH)
