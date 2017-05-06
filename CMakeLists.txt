cmake_minimum_required(VERSION 3.0)
project(cross_device_interaction)

set(CMAKE_CXX_FLAGS "-std=c++14 -D__LINUX__ -Wall -Wextra -Werror -pedantic")

set(BUILD_VISUALIZATION_TOOLS false CACHE BOOL "Build visualiztion tools")

if(${BUILD_VISUALIZATION_TOOLS})
    find_package(OpenCV REQUIRED CONFIG)
    find_package(VTK REQUIRED CONFIG)
endif()

add_subdirectory(demos)
add_subdirectory(sensor)
add_subdirectory(examples)
add_subdirectory(tools)
