cmake_minimum_required(VERSION 3.19)
project(shared_ptr_h)

set(CMAKE_CXX_STANDARD 17)
include_directories(${shared_ptr_h_SOURCE_DIR})
add_executable(shared_ptr_h main.cpp)