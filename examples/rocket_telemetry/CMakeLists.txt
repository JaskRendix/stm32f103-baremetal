add_executable(rocket_telemetry
    main.c
    telemetry.c
    flight_sim.c
)

target_include_directories(rocket_telemetry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
