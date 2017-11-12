cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 11)

project(radixtree)
add_executable(rttest src/main.cpp)

include_directories(include)

install(TARGETS rttest DESTINATION bin)
