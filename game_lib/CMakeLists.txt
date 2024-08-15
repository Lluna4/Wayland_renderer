cmake_minimum_required(VERSION 3.28)
project(game_lib C)

set(CMAKE_C_STANDARD 11)

add_library(game_lib STATIC library.c)
