cmake_minimum_required(VERSION 3.10)
project(hardware)

set(SOURCE src)

set(INCLUDES inc)
set(LIBRARIES ws2811)

# hardware
add_library(
    hardware
    STATIC
    src/LEDController.cpp
    src/OFController.cpp
)
target_include_directories(
    hardware
    PUBLIC
    ${INCLUDES}
)
target_link_libraries(
    hardware
    ${LIBRARIES}
)
