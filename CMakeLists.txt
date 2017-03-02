cmake_minimum_required(VERSION 3.0)
project(cross_device_interaction)

set(CMAKE_CXX_FLAGS "-std=c++14 -D__LINUX__ -Wall -Wextra -Werror -pedantic")

add_subdirectory(sensor)
add_subdirectory(examples)
