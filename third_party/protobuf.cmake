set(SRC_DIR ../Protobuf/)

ExternalProject_Add(Protobuf
  PREFIX                protobuf
  GIT_REPOSITORY        https://github.com/google/protobuf
  GIT_TAG               v3.6.0.1

  SOURCE_SUBDIR         ${SRC_DIR}/cmake

  CMAKE_CACHE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX:PATH=${EXT_INSTALL_DIR}
      -DCMAKE_INSTALL_RPATH:PATH=${EXT_INSTALL_DIR}/lib
      -DCMAKE_PREFIX_PATH:PATH=${EXT_INSTALL_DIR}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -Dprotobuf_BUILD_SHARED_LIBS_DEFAULT:BOOL=ON
      -Dprotobuf_BUILD_TESTS:BOOL=OFF
      -Dprotobuf_BUILD_EXAMPLES:BOOL=OFF
)

include_directories(${protobuf_INCLUDE_DIRS})
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
