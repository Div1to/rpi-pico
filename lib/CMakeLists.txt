pico_add_library(gpio)

target_sources(gpio INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/gpio_i2c.c
)

target_include_directories(gpio_headers INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

pico_mirrored_target_link_libraries(gpio INTERFACE
    hardware_gpio
    pico_stdlib
)