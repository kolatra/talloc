cmake_minimum_required(VERSION 3.8)

project("malloc")

add_executable(malloc "malloc.c" "util.c" "util.h")

set(CMAKE_BUILD_TYPE debug)