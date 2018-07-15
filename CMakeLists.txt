cmake_minimum_required(VERSION 3.11)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif(CCACHE_PROGRAM)

project(ipme)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-D__LINUX__ -DBOOST_ALL_DYN_LINK -Wall -Wextra -Werror")

# set some ridiculous parameters so we don't get ridiculous errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses")

set(THIRD_PARTY_ROOT "${CMAKE_CURRENT_BINARY_DIR}/third_party" CACHE PATH
    "Path of the 3rd party root directory")

set(third_party_dir "${THIRD_PARTY_ROOT}/install")
set(CMAKE_INSTALL_RPATH "${third_party_dir}/lib")

set(QT5_ROOT CACHE PATH "Path to Qt5 installation")

set(BOOST_ROOT ${third_party_dir})
set(VTK_DIR ${third_party_dir}/lib/cmake/vtk-8.1)
set(OpenCV_DIR ${third_party_dir}/share/OpenCV)

set(OMICRON_SOURCE_PATH ${THIRD_PARTY_ROOT}/omicron/src/Omicron)
set(Qt5Core_DIR "${QT5_ROOT}/lib/cmake/Qt5Core")
set(Qt5Widgets_DIR "${QT5_ROOT}/lib/cmake/Qt5Widgets")

message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message("BOOST_ROOT=${BOOST_ROOT}")
message("VTK_DIR=${VTK_DIR}")
message("OpenCV_DIR=${OpenCV_DIR}")
message("OMICRON_SOURCE_PATH=${OMICRON_SOURCE_PATH}")

set(BUILD_VISUALIZATION_TOOLS false CACHE BOOL "Build visualiztion tools")
set(BUILD_SENSOR_WALABOT false CACHE BOOL "Build code that uses Walabot SDK")
set(BUILD_VICON_CODE true CACHE BOOL "Build code using VICON motion capture")
set(BUILD_TESTS false CACHE BOOL "Build unittests")

set(ipmelib_DIR ${CMAKE_SOURCE_DIR}/lib)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED COMPONENTS system filesystem log_setup log
    program_options thread)
find_package(OpenCV REQUIRED CONFIG)
find_package(VTK REQUIRED CONFIG)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${VTK_INCLUDE_DIRS})

if(${BUILD_SENSOR_WALABOT})
    include_directories(${CMAKE_SOURCE_DIR}/sensor)
endif()

add_subdirectory(lib)
add_subdirectory(demos)
add_subdirectory(tools)

if(${BUILD_TESTS})
    set(GTEST_BUILD_DIR CACHE PATH "Path to GTest Libraries")
    add_subdirectory(tests)
endif()
