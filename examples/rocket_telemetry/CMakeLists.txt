add_executable(rocket_telemetry
    main.c
    telemetry.c
    flight_sim.c
    blackbox.c
    ${CMAKE_SOURCE_DIR}/drivers/flash.c
    ${CMAKE_SOURCE_DIR}/tests/flash_fake_hw.c
)

target_include_directories(rocket_telemetry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tests
)

target_compile_options(rocket_telemetry PRIVATE
    -include${CMAKE_SOURCE_DIR}/tests/flash_fake_hw.h
)