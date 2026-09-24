cmake_minimum_required(VERSION 3.20)

project(tri3d_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(tri3d_engine
    src/main.cpp
    src/Engine.cpp
    src/Scene.cpp
    src/Camera.cpp
    src/UniformGrid.cpp
    src/IntersectionSystem.cpp
    src/Renderer.cpp
)

target_include_directories(tri3d_engine PRIVATE include)

if(MSVC)
    target_compile_options(tri3d_engine PRIVATE /W4 /permissive-)
else()
    target_compile_options(tri3d_engine PRIVATE -Wall -Wextra -Wpedantic)
endif()
