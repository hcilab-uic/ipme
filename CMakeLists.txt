cmake_minimum_required(VERSION 3.11)
project(ipme)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-D__LINUX__ -DBOOST_ALL_DYN_LINK -Wall -Wextra -Werror")

set(THIRD_PARTY_DIR "${CMAKE_CURRENT_BINARY_DIR}/third_party/third_party"
  CACHE PATH "Path to the third-party build directory")

set(BOOST_ROOT ${THIRD_PARTY_DIR})
set(VTK_DIR ${THIRD_PARTY_DIR}/lib/cmake/vtk-8.1)
set(OpenCV_DIR ${THIRD_PARTY_DIR}/share/OpenCV)
set(Protobuf_INCLUDE_DIR ${THIRD_PARTY_DIR}/include)
set(Protobuf_LIBRARY ${THIRD_PARTY_DIR}/lib/libprotobuf.a)
set(OMICRON_SOURCE_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/third_party/omicron/src/Omicron)

message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message("BOOST_ROOT=${BOOST_ROOT}")
message("VTK_DIR=${VTK_DIR}")
message("OpenCV_DIR=${OpenCV_DIR}")
message("OMICRON_SOURCE_PATH=${OMICRON_SOURCE_PATH}")

SET(CMAKE_EXE_LINKER_FLAGS
  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,${THIRD_PARTY_DIR}/lib")

# set(CMAKE_INSTALL_RPATH "${THIRD_PARTY_DIR}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(Protobuf REQUIRED)


set(BUILD_VISUALIZATION_TOOLS false CACHE BOOL "Build visualiztion tools")
set(BUILD_SENSOR_WALABOT false CACHE BOOL "Build code that uses Walabot SDK")
set(BUILD_VICON_CODE true CACHE BOOL "Build code using VICON motion capture")
set(BUILD_TESTS false CACHE BOOL "Build unittests")

set(ipmelib_DIR ${CMAKE_SOURCE_DIR}/lib)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BUILD_SHARED_LIBS ON)

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
