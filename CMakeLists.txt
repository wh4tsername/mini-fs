cmake_minimum_required(VERSION 3.17)
project(mini_fs C)

set(CMAKE_C_STANDARD 11)

add_executable(mini_fs main.c client.c handlers.c)
