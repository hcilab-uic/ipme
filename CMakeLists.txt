cmake_minimum_required(VERSION 3.13)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif(CCACHE_PROGRAM)

project(proxe)

set(CMAKE_CXX_STANDARD 20)
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

if(MSVC)
    set(OpenCV_DIR ${third_party_dir}/lib)
else()
    set(OpenCV_DIR ${third_party_dir}/lib/cmake/opencv4)
endif(MSVC)

set(OMICRON_SOURCE_PATH ${THIRD_PARTY_ROOT}/omicron/src/Omicron)
set(Qt5Core_DIR "${QT5_ROOT}/lib/cmake/Qt5Core")
set(Qt5Widgets_DIR "${QT5_ROOT}/lib/cmake/Qt5Widgets")
set(Qt53DCore_DIR "${QT5_ROOT}/lib/cmake/Qt53DCore")
set(Qt53DExtras_DIR "${QT5_ROOT}/lib/cmake/Qt53DExtras")

set(dlib_DIR "${third_party_dir}/lib/cmake/dlib")

message(STATUS "THIRD_PARTY_ROOT=${THIRD_PARTY_ROOT}")
message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message("BOOST_ROOT=${BOOST_ROOT}")
message("OpenCV_DIR=${OpenCV_DIR}")
message("OMICRON_SOURCE_PATH=${OMICRON_SOURCE_PATH}")
message(STATUS "Qt53DCore_DIR=${Qt53DCore_DIR}")

set(BUILD_EXAMPLES true CACHE BOOL "Build example code")
set(BUILD_MOCAP_CODE true CACHE BOOL "Build motion capture code")
set(BUILD_TESTS false CACHE BOOL "Build unittests")

set(ipmelib_DIR ${CMAKE_SOURCE_DIR}/lib)


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_ARCHITECTURE "-x64")

find_package(Boost REQUIRED COMPONENTS
    date_time
    system
    filesystem
    log_setup
    log
    program_options
    thread)

find_package(OpenCV REQUIRED CONFIG)
find_package(dlib REQUIRED CONFIG)

include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/sensor)

add_subdirectory(lib)
add_subdirectory(tools)

if(${BUILD_TESTS})
    set(GTEST_BUILD_DIR CACHE PATH "Path to GTest Libraries")
    add_subdirectory(tests)
endif()

if(${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()
