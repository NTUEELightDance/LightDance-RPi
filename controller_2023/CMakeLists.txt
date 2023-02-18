cmake_minimum_required(VERSION 3.10)
project(controller_2023)

set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++17 -Wall"
)

add_subdirectory(controller)
add_subdirectory(hardware)
