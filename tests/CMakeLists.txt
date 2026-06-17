add_executable(test_gpio
    gpio_fake_hw.c
    ../drivers/gpio.c
    test_gpio.c
)

target_include_directories(test_gpio PRIVATE
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tests
)

set_target_properties(test_gpio PROPERTIES
    C_STANDARD 11
)
