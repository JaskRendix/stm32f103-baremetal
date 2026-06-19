cmake_minimum_required(VERSION 3.15)

add_executable(morse_code
    main.c
    ${CMAKE_SOURCE_DIR}/drivers/gpio.c
)

target_include_directories(morse_code PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/core
)

add_custom_command(TARGET morse_code POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:morse_code> $<TARGET_FILE_DIR:morse_code>/morse_code.bin
)
