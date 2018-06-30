set(SRC_DIR ../Boost/)

ExternalProject_Add(Boost
  PREFIX                boost
  GIT_REPOSITORY        https://github.com/boostorg/boost.git
  GIT_TAG               boost-1.67.0
  
  CMAKE_ARGS            -DBUILD_SHARED_LIBS=ON
  
  CONFIGURE_COMMAND    cd ${SRC_DIR} && pwd && ./bootstrap.sh
  BUILD_COMMAND
      cd ${SRC_DIR} &&
      pwd &&
      ./b2 -j8 install --prefix=${EXT_INSTALL_DIR}
  INSTALL_COMMAND      ""
)

# ExternalProject_Get_Property(Protobuf EXT_INSTALL_DIR)
message(STATUS "Install directory ${EXT_INSTALL_DIR}")
