cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

#find cppzmq wrapper, installed by make of cppzmq
find_package(cppzmq)

# set the project name
project(test_receiver)

# add the executable
add_executable(test_receiver receiver.cpp)
target_link_libraries(test_receiver cppzmq)