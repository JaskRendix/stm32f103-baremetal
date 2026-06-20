cmake_minimum_required(VERSION 3.15)
project(stm32f103-baremetal C ASM)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" CACHE STRING "" FORCE)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Host-side example programs are off by default so that configuring
# build_tests (host unit tests) doesn't also compile every example.
# Enabled explicitly by `make examples` via -DBUILD_EXAMPLES=ON.
option(BUILD_EXAMPLES "Build host-side example programs" OFF)

set(SOURCES
    core/startup.c
    core/vectors.c
    drivers/gpio.c
    drivers/systick.c
    drivers/uart.c
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

if(NOT CMAKE_CROSSCOMPILING)
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

    add_executable(test_systick
        tests/systick_fake_hw.c
        tests/test_systick.c
        drivers/systick.c
    )
    set_target_properties(test_systick PROPERTIES
        C_STANDARD 99
    )
    target_include_directories(test_systick PRIVATE
        drivers
        include
        core
        tests
    )
    target_compile_options(test_systick PRIVATE
        -include ${CMAKE_SOURCE_DIR}/tests/systick_fake_hw.h
    )

    add_executable(test_uart
        tests/uart_fake_hw.c
        tests/gpio_fake_hw.c
        tests/test_uart.c
        drivers/uart.c
        drivers/gpio.c
    )
    set_target_properties(test_uart PROPERTIES
        C_STANDARD 99
    )
    target_include_directories(test_uart PRIVATE
        drivers
        include
        core
        tests
    )
    target_compile_options(test_uart PRIVATE
        -include${CMAKE_SOURCE_DIR}/tests/gpio_fake_hw.h
        -include${CMAKE_SOURCE_DIR}/tests/uart_fake_hw.h
    )
endif()

if(NOT CMAKE_CROSSCOMPILING AND BUILD_EXAMPLES)
    add_subdirectory(examples/led_shift)
    add_subdirectory(examples/morse_code)
    add_subdirectory(examples/mosquito_trap)
    add_subdirectory(examples/cpu8_vm)
    add_subdirectory(examples/rocket_telemetry)
endif()