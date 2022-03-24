# the name of the target operating system
set(CMAKE_SYSTEM_NAME Generic)

set(DOCKER_ADAPTER "${CMAKE_CURRENT_LIST_DIR}/script/m68k-tools.py")

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   "${CMAKE_CURRENT_LIST_DIR}/script/m68k-elf-gcc.py")
# C++ currently not working
# set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_LIST_DIR}/script/m68k-elf-g++.py")

add_compile_options(
    -m68000
)

add_link_options(
    -nostartfiles -nodefaultlibs                # Exclude various libraries and such that we don't need
    -T "${CMAKE_CURRENT_LIST_DIR}/link.ld"      # Use a custom, simple linker script for our target board
)

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_BIN})


# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)