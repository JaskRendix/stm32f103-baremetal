cmake_minimum_required(VERSION 3.15)

add_executable(led_shift
    main.c
    ${CMAKE_SOURCE_DIR}/drivers/gpio.c
)

target_include_directories(led_shift PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/core
)

add_custom_command(TARGET led_shift POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:led_shift> $<TARGET_FILE_DIR:led_shift>/led_shift.bin
)
