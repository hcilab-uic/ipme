cmake_minimum_required(VERSION 3.11)
project(ipme)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Unfortunately VTK headers have a lot of errors with -pedantic flag. So
# removing -pedantic for now
set(CMAKE_CXX_FLAGS "-D__LINUX__ -DBOOST_LOG_DYN_LINK -Wall -Wextra -Werror")

set(BUILD_VISUALIZATION_TOOLS false CACHE BOOL "Build visualiztion tools")
set(BUILD_SENSOR_WALABOT false CACHE BOOL "Build code that uses Walabot SDK")
set(BUILD_VICON_CODE false CACHE BOOL "Build code using VICON motion capture")
set(BUILD_TESTS false CACHE BOOL "Build unittests")

set(ipmelib_DIR ${CMAKE_SOURCE_DIR}/lib)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} /opt/software/Qt/5.11.0/5.11.0")
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

if(${BUILD_VICON_CODE})
    set(OMICRON_SOURCE_PATH CACHE PATH "Path to Omicron SDK")
endif()

add_subdirectory(lib)

add_subdirectory(demos)
add_subdirectory(tools)

if(${BUILD_TESTS})
    set(GTEST_BUILD_DIR CACHE PATH "Path to GTest Libraries")
    add_subdirectory(tests)
endif()
