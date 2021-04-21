# local variables
set(TARGET_NAME app.elf)

# target
file(GLOB C_FILES *.c)
file(GLOB ASM_FILES *.S)

## deal with asm files
foreach(X IN LISTS ASM_FILES)
    set_property(SOURCE ${X} PROPERTY LANGUAGE C)
endforeach()

add_executable(${TARGET_NAME} ${C_FILES} ${ASM_FILES})

target_link_options(${TARGET_NAME} PRIVATE -T ${CMAKE_SOURCE_DIR}/app/linker.ld)