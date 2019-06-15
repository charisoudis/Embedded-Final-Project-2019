cmake_minimum_required(VERSION 3.13)
project(Final C)

set(CMAKE_C_STANDARD 99)

add_executable(Final main.c c/client.c h/client.h c/server.c h/server.h c/utils.c h/utils.h)