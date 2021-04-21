include_directories(${CMAKE_SOURCE_DIR}/include/bootloader ${CMAKE_SOURCE_DIR}/include/lib)

# local variables
set(TARGET_NAME "bootloader.elf")

# asm file
set_property(SOURCE ./src/start.S PROPERTY LANGUAGE C)

# target
file(GLOB C_FILES src/*.c)
file(GLOB ASM_FILES src/*.S)

add_executable(${TARGET_NAME} ${C_FILES} ${ASM_FILES})

target_link_options(${TARGET_NAME} PRIVATE -T ${CMAKE_SOURCE_DIR}/bootloader/src/linker.ld)


target_link_libraries(
    ${TARGET_NAME}
    mini_uart
    base_ops
    string
    math
    io
)
