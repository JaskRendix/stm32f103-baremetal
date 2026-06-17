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

if(CMAKE_C_COMPILER MATCHES "arm-none-eabi-gcc")

    message(STATUS "Building firmware for ARM Cortex-M3")

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

endif()

set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")

add_executable(test_gpio
    tests/gpio_fake_hw.c
    tests/test_gpio.c
    drivers/gpio.c
)

set_target_properties(test_gpio PROPERTIES
    C_STANDARD 99
)

target_include_directories(test_gpio PRIVATE
    drivers
    include
    core
    tests
)


target_compile_options(test_gpio PRIVATE
    -include ${CMAKE_SOURCE_DIR}/tests/gpio_fake_hw.h
)

