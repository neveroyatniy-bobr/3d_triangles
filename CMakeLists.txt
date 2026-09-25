cmake_minimum_required(VERSION 3.20)

project(
    tri3d_engine
    VERSION 0.1.0
    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ------------------------------------------------------------
# Engine core
# ------------------------------------------------------------

add_library(tri3d_core

    src/Vec3.cpp
    src/Triangle.cpp

    src/Camera.cpp
    src/UniformGrid.cpp
    src/IntersectionSystem.cpp
    src/Renderer.cpp
    src/Engine.cpp
)


target_include_directories(
    tri3d_core
    PUBLIC
        include
    PRIVATE
        src
)


# ------------------------------------------------------------
# Compiler warnings
# ------------------------------------------------------------

if(MSVC)

    target_compile_options(
        tri3d_core
        PRIVATE
            /W4
            /permissive-
    )

else()

    target_compile_options(
        tri3d_core
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )

endif()


# ------------------------------------------------------------
# Main executable
# ------------------------------------------------------------

add_executable(
    tri3d_engine
    src/main.cpp
)


target_link_libraries(
    tri3d_engine
    PRIVATE
        tri3d_core
)


# ------------------------------------------------------------
# Tests
# ------------------------------------------------------------

include(CTest)


if(BUILD_TESTING)

    add_executable(
        tri3d_tests
        tests/TriangleTests.cpp
    )


    target_link_libraries(
        tri3d_tests
        PRIVATE
            tri3d_core
    )


    add_test(
        NAME TriangleIntersectionTests
        COMMAND tri3d_tests
    )

endif()