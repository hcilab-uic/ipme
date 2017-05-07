cmake_minimum_required(VERSION 3.0)
project(cross_device_interaction)

# Unfortunately VTK headers have a lot of errors with -pedantic flag. So
# removing -pedantic for now
set(CMAKE_CXX_FLAGS "-std=c++14 -D__LINUX__ -DBOOST_LOG_DYN_LINK -Wall -Wextra -Werror")
set(BUILD_VISUALIZATION_TOOLS false CACHE BOOL "Build visualiztion tools")
set(BUILD_SENSOR_WALABOT false CACHE BOOL "Build code that uses Walabot SDK")

set(cdilib_DIR ${CMAKE_SOURCE_DIR}/lib)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED COMPONENTS system filesystem log_setup log
    program_options)

if(${BUILD_VISUALIZATION_TOOLS})
    find_package(OpenCV REQUIRED CONFIG)
    find_package(VTK REQUIRED CONFIG)

    include_directories(${OpenCV_INCLUDE_DIRS})
    include_directories(${VTK_INCLUDE_DIRS})
endif()

if(${BUILD_SENSOR_WALABOT})
    include_directories(${CMAKE_SOURCE_DIR}/sensor)
endif()

add_subdirectory(lib)

add_subdirectory(demos)
add_subdirectory(examples)
add_subdirectory(tools)
