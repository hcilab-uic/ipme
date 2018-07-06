set(SRC_DIR ../Protobuf/)

ExternalProject_Add(Protobuf
  PREFIX                protobuf
  GIT_REPOSITORY        https://github.com/google/protobuf
  GIT_TAG               v3.6.0.1

  CMAKE_ARGS            -DBUILD_SHARED_LIBS=ON
  CMAKE_CACHE_ARGS
    -Dprotobuf_BUILD_TESTS:BOOL=OFF
    -Dprotobuf_BUILD_EXAMPLES:BOOL=OFF

  CONFIGURE_COMMAND
    cmake ${SRC_DIR}/cmake
      -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR}
)

# ExternalProject_Get_Property(Protobuf EXT_INSTALL_DIR)
message(STATUS "Install directory ${EXT_INSTALL_DIR}")
