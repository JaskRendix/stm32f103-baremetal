cmake_minimum_required(VERSION 3.15)

add_executable(cpu8_vm
    main.c
    cpu8.c
)

target_include_directories(cpu8_vm PRIVATE
    ${CMAKE_SOURCE_DIR}/examples/cpu8_vm
)
