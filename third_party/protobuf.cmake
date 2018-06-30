set(SRC_DIR ../Protobuf/)

ExternalProject_Add(Protobuf
  PREFIX                protobuf
  GIT_REPOSITORY        https://github.com/google/protobuf
  GIT_TAG               v3.6.0.1
  
  CMAKE_ARGS            -DBUILD_SHARED_LIBS=ON
  CMAKE_CACHE_ARGS      -Dprotobuf_BUILD_TESTS:BOOL=OFF
  
  CONFIGURE_COMMAND     cmake -GNinja ${SRC_DIR}/cmake -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR}
  BUILD_COMMAND         ninja
  INSTALL_COMMAND       ninja install
)

# ExternalProject_Get_Property(Protobuf EXT_INSTALL_DIR)
message(STATUS "Install directory ${EXT_INSTALL_DIR}")
