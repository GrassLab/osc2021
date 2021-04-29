# META
cmake_minimum_required(VERSION 3.16)
project(OSC2021)

# global variables
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_C_FLAGS "-Wall -nostdlib -ffreestanding -Werror")

if (${DEBUG})
    message("\n######### WARNING: THIS iS DEBUG MODE #########\n")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -ggdb3 -O0")
    
    # debug definition
    add_compile_definitions(EMU=1)
endif(${DEBUG})

# library
include_directories(lib/include)

## asm file
set_property(SOURCE ${LIB_DIR}/src/base_ops.S PROPERTY LANGUAGE C)
set_property(SOURCE ${LIB_DIR}/src/system_call.S PROPERTY LANGUAGE C)

add_library(base_ops STATIC ${LIB_DIR}/src/base_ops.S)
add_library(system_call STATIC ${LIB_DIR}/src/system_call.S)

add_library(io STATIC ${LIB_DIR}/src/io.c)
add_library(math STATIC ${LIB_DIR}/src/math.c)
add_library(mini_uart STATIC ${LIB_DIR}/src/mini_uart.c)
add_library(string STATIC ${LIB_DIR}/src/string.c)

# targets
add_subdirectory(bootloader)
add_subdirectory(kernel)
add_subdirectory(app)
add_subdirectory(argv_test)