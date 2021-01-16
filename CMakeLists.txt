cmake_minimum_required(VERSION 3.17)
project(crenderer C)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(crenderer src/main.c src/display.h src/display.c src/input.h src/input.c)

target_link_libraries(crenderer ${SDL2_LIBRARY})