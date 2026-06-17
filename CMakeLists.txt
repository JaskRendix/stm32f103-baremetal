cmake_minimum_required(VERSION 3.15)
project(stm32f103-baremetal C ASM)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" CACHE STRING "" FORCE)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES
    core/startup.c
    core/vectors.c
    drivers/gpio.c
    src/main.c
)

add_executable(out.elf ${SOURCES})

target_include_directories(out.elf PRIVATE
    include
    core
    drivers
)

target_compile_options(out.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    --specs=nano.specs
    -fdata-sections
    -ffunction-sections
    -Wall
    -Werror
    -Wfatal-errors
    $<$<CONFIG:Debug>:-O0 -g>
)

target_link_options(out.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/core/linker.ld
    -mcpu=cortex-m3
    -mthumb
    --specs=nano.specs
    -lc -lm -lnosys
    -Wl,--gc-sections
)
