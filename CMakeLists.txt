cmake_minimum_required(VERSION 3.20)

project(tri3d LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================
# Core
# ============================================================

add_library(
    tri3d_core

    src/Vec3.cpp
    src/Triangle.cpp
    src/UniformGrid.cpp
    src/IntersectionSystem.cpp
)

target_include_directories(
    tri3d_core

    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


# ============================================================
# Main executable
# ============================================================

add_executable(
    tri3d_engine

    src/main.cpp
)

target_link_libraries(
    tri3d_engine

    PRIVATE
        tri3d_core
)


# ============================================================
# Tests
# ============================================================

include(CTest)

if(BUILD_TESTING)

    # --------------------------------------------------------
    # Triangle intersection tests
    # --------------------------------------------------------

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


    # --------------------------------------------------------
    # UniformGrid + IntersectionSystem randomized tests
    # --------------------------------------------------------

    add_executable(
        tri3d_intersection_system_tests

        tests/IntersectionSystemTests.cpp
    )

    target_link_libraries(
        tri3d_intersection_system_tests

        PRIVATE
            tri3d_core
    )

    add_test(
        NAME IntersectionSystemRandomizedTests
        COMMAND tri3d_intersection_system_tests
    )

endif()