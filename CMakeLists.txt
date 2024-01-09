
cmake_minimum_required(VERSION 3.13)

include(${CMAKE_PICO_SDK})

project(prj_rpi_pico)

pico_sdk_init()

pico_add_subdirectory(lib)
#pico_add_subdirectory(src)

add_executable(prj_rpi_pico
    main.c
)

target_link_libraries(prj_rpi_pico
    gpio
)