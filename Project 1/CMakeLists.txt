cmake_minimum_required(VERSION 3.13)
project(project1 C)

set(CMAKE_C_STANDARD 99)

add_executable(project1 main.c sampling_gettimeofday.c sampling_gettimeofday.h stats.c stats.h sampling_cputime.c sampling_cputime.h)
target_link_libraries(project1 m)

set(GCC_COVERAGE_COMPILE_FLAGS "-lm")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )