cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 11)

project(radixtree)
add_executable(prefix_test src/prefix_test.cpp)

include_directories(include)

install(TARGETS prefix_test DESTINATION bin)
