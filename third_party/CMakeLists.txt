cmake_minimum_required(VERSION 3.11)
project(ipme_externals)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(ExternalProject)

set(EXT_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party)

include(boost.cmake)
include(protobuf.cmake)
include(vtk.cmake)
include(opencv.cmake)
include(omicron.cmake)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH)

message("Important: Third-party install directory: ${EXT_INSTALL_DIR}")
