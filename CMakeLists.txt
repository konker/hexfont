cmake_minimum_required(VERSION 2.8)
project(hexfont)
set(CMAKE_BUILD_TYPE Debug)

include_directories(include)

file(GLOB LIBSOURCES "src/*.c")
file(GLOB EXAMPLESOURCES "examples/*.c")

add_library(hexfont_static STATIC ${LIBSOURCES})
add_library(hexfont_shared SHARED ${LIBSOURCES})

add_executable(example examples/example.c)
target_link_libraries(example hexfont_static)
