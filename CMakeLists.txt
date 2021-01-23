cmake_minimum_required(VERSION 3.17)
project(crenderer C)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(crenderer src/main.c src/gfx/display.h src/gfx/display.c src/input.h src/input.c src/gfx/draw.c src/gfx/draw.h src/math/vector.c src/math/vector.h src/scene/scene.c src/scene/scene.h src/scene/mesh.c src/scene/mesh.h)

target_link_libraries(crenderer ${SDL2_LIBRARY})