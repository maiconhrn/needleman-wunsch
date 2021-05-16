cmake_minimum_required(VERSION 3.17)
project(needleman_wusnch)

set(CMAKE_CXX_STANDARD 11)

add_executable(nw src/main.cpp
        src/nw/needleman-wunsch.hpp
        src/nw/needleman-wunsch.cpp
        src/nw/seq/needleman-wunsch-seq.cpp
        src/nw/par/needleman-wunsch-par.cpp)

target_link_libraries(nw pthread)