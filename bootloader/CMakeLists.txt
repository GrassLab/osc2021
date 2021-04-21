include_directories(${CMAKE_SOURCE_DIR}/bootloader/include ${CMAKE_SOURCE_DIR}/lib/include)

# local variables
set(TARGET_NAME bootloader.elf)

# target
file(GLOB C_FILES src/*.c)
file(GLOB ASM_FILES src/*.S)

## deal with asm files
foreach(X IN LISTS ASM_FILES)
    set_property(SOURCE ${X} PROPERTY LANGUAGE C)
endforeach()

add_executable(${TARGET_NAME} ${C_FILES} ${ASM_FILES})

target_link_options(${TARGET_NAME} PRIVATE -T ${CMAKE_SOURCE_DIR}/bootloader/src/linker.ld)


# link
target_link_libraries(
    ${TARGET_NAME}
    mini_uart
    base_ops
    string
)
