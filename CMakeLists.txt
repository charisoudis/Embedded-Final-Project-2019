cmake_minimum_required(VERSION 3.13)
project(Final C)

set(CMAKE_C_STANDARD 99)

add_executable(Final app.c client.c client.h server.c server.h utils.c utils.h)