cmake_minimum_required(VERSION 3.16)
project(kp)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
set(CMAKE_CXX_COMPILER "clang++")


add_library(lzw SHARED
        lzw.cpp
        lzw.hpp)

add_library(Arithmetic SHARED
        huffman.cpp
        huffman.hpp)

add_executable(kp
        main.cpp )

target_link_libraries(kp lzw Arithmetic)

add_executable(kek kek.cpp)