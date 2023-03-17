cmake_minimum_required(VERSION 3.10)
project(controller)

set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++17 -Wall"
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(software)
add_subdirectory(hardware)
