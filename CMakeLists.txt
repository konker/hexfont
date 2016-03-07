cmake_minimum_required(VERSION 2.8)
project(hexfont)
set(CMAKE_BUILD_TYPE Debug)

option(SHARED_LIBRARY "Build a shared library" OFF)

include_directories(include)

file(GLOB LIBSOURCES "src/*.c")
file(GLOB EXAMPLESOURCES "examples/*.c")

add_library(hexfont ${LIBSOURCES})

add_executable(hexfont_example examples/hexfont_example.c)
target_link_libraries(hexfont_example hexfont)
