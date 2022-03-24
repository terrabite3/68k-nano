cmake_minimum_required(VERSION 3.22.2)

project(test C)


add_executable(${PROJECT_NAME}
    src/test.c
    src/uart.c
    src/sleep.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
)

# target_compile_options(${PROJECT_NAME}
#     # -O1 or higher will not work until you fix your linker script!
#     PRIVATE -O0 
# )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${DOCKER_ADAPTER} m68k-elf-objcopy -O binary $<TARGET_FILE:test> $<TARGET_FILE_BASE_NAME:test>.bin
    COMMAND dd if=/dev/zero bs=1 count=16384 >>$<TARGET_FILE_BASE_NAME:test>.bin
)


add_custom_target("load"
    python3 ${CMAKE_SOURCE_DIR}/script/serload.py $<TARGET_FILE_BASE_NAME:test>.bin
    DEPENDS test
)

add_custom_target("objdump"
    ${DOCKER_ADAPTER} m68k-elf-objdump -d $<TARGET_FILE:test>
    DEPENDS test
)
