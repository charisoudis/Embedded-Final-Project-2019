cmake_minimum_required(VERSION 3.13)
project(Final C)

set(CMAKE_C_STANDARD 99)

include_directories(h)
add_subdirectory(c)

target_link_libraries(Final FINAL_LIB)