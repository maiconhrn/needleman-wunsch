cmake_minimum_required(VERSION 3.17)
project(needleman_wusnch)

set(CMAKE_CXX_STANDARD 11)

add_executable(nw src/main.cpp
        src/needleman-wunsch.hpp
        src/needleman-wunsch.cpp)

target_link_libraries(nw pthread)