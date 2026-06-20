add_executable(test_systick
    systick_fake_hw.c
    ../drivers/systick.c
    test_systick.c
)

target_include_directories(test_systick PRIVATE
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tests
)

set_target_properties(test_systick PROPERTIES
    C_STANDARD 11
)
