cmake_minimum_required(VERSION 3.17)
project(needleman_wusnch)

set(CMAKE_CXX_STANDARD 11)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -funroll-loops -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(nw src/main.cpp
        src/nw/needleman-wunsch.hpp
        src/nw/needleman-wunsch.cpp
        src/nw/seq/needleman-wunsch-seq.cpp
        src/nw/par/needleman-wunsch-par.cpp)

# MPI
find_package(MPI REQUIRED)
target_link_libraries(nw PUBLIC pthread MPI::MPI_CXX)