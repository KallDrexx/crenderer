cmake_minimum_required(VERSION 3.17)
project(crenderer C)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(crenderer src/main.c src/gfx/display.h src/gfx/display.c src/input.h src/input.c src/math/vector.c src/math/vector.h src/scene/scene.c src/scene/scene.h src/scene/mesh.c src/scene/mesh.h src/gfx/renderer.c src/gfx/renderer.h src/math/angles.h src/list.c src/list.h src/math/matrix.c src/math/matrix.h src/gfx/triangle_renderer.c src/gfx/triangle_renderer.h src/gfx/render_settings.h src/scene/lighting.h src/scene/texture.c src/scene/texture.h src/scene/upng.c src/scene/upng.h src/scene/camera.c src/scene/camera.h src/gfx/clipping.c src/gfx/clipping.h)

target_link_libraries(crenderer ${SDL2_LIBRARY})

add_custom_command(
        TARGET crenderer PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets
                ${CMAKE_CURRENT_BINARY_DIR}/assets
)

SET_PROPERTY(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/assets)
