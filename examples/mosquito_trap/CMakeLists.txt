cmake_minimum_required(VERSION 3.15)

add_executable(mosquito_trap
    main.c
    ${CMAKE_SOURCE_DIR}/drivers/gpio.c
)

target_include_directories(mosquito_trap PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/core
)

add_custom_command(TARGET mosquito_trap POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:mosquito_trap> $<TARGET_FILE_DIR:mosquito_trap>/mosquito_trap.bin
)
