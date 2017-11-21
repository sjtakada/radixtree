# 
cmake_minimum_required(VERSION 2.8)
project(radixtree)

set (CMAKE_CXX_STANDARD 11)

#enable_testing()
find_package(GTest REQUIRED)
include_directories(include ${GTEST_INCLUDE_DIRS})

add_executable(prefix_test src/prefix_test.cpp)
add_executable(tree_test src/tree_test.cpp)

target_link_libraries(prefix_test ${GTEST_LIBRARIES} pthread)
