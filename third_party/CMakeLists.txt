cmake_minimum_required(VERSION 3.13)
project(ipme_third_party)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS
    "Setting build type to '${default_build_type}' as none was specified.")

  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
    STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(ExternalProject)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(EXT_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install)
set(CMAKE_INSTALL_RPATH ${EXT_INSTALL_DIR}/lib)

if(MSVC)
  set(ZLIB_INCLUDE_DIR "${ZLIB_ROOT}/include")
  set(ZLIB_LIBRARY "${ZLIB_ROOT}/lib/zlib.lib")
  message(STATUS "Building for Microsoft platform,
      set ZLIB_LIBRARY=${ZLIB_LIBRARY}, ZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}")
endif(MSVC)

include(boost.cmake)
include(dlib.cmake)
include(opencv.cmake)
include(omicron.cmake)
include(protobuf.cmake)

message("Important: Third-party install directory: ${EXT_INSTALL_DIR}")
