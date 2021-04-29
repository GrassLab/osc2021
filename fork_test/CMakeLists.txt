include_directories(${CMAKE_SOURCE_DIR}/lib/include)

# local variables
set(TARGET fork_test.elf)

# target
file(GLOB C_FILES *.c)
file(GLOB ASM_FILES *.S)

## deal with asm files
foreach(X IN LISTS ASM_FILES)
    set_property(SOURCE ${X} PROPERTY LANGUAGE C)
endforeach()

add_executable(${TARGET} ${C_FILES} ${ASM_FILES})

target_link_options(${TARGET} PRIVATE -T ${CMAKE_SOURCE_DIR}/fork_test/linker.ld)

target_link_libraries(
    ${TARGET}
    io
    mini_uart
    base_ops
    string
    system_call
)
