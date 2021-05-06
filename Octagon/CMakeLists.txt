cmake_minimum_required(VERSION 3.19)
project(Octagon)

set(CMAKE_CXX_STANDARD 17)

include_directories(Octagon_SOURCE_DIR)

add_executable(Octagon main.cpp)