cmake_minimum_required(VERSION 3.9)

project("malloc")

# add_executable(malloc "malloc.c" "util.c" "util.h")
add_executable(malloc_test "malloc_test.c" "malloc.c" "malloc.h" "util.c" "util.h")

set(CMAKE_CXX_FLAGS "-Wall -Wextra" "-Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_BUILD_TYPE debug)
# set(CMAKE_BUILD_TYPE Release)

# add_library(mylib SHARED malloc.c util.c util.h)

# set_target_properties(malloc PROPERTIES
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
# )
