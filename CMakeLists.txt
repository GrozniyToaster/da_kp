cmake_minimum_required(VERSION 3.16)
project(kp)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
set(CMAKE_CXX_COMPILER "clang++")


add_library(lzw SHARED
        lzw.cpp
        lzw.hpp
        utils.hpp)



add_library(Huffman SHARED
        huffman.cpp
        huffman.hpp
        utils.hpp)



add_executable(kp
        main.cpp)

target_link_libraries(kp lzw Huffman)


add_executable(kek kek.cpp)