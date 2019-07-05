cmake_minimum_required(VERSION 3.13)
project(Final C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(Final ${SOURCE_FILES})

include_directories(include)
include_directories(src)

add_subdirectory(src)
#add_subdirectory(test)