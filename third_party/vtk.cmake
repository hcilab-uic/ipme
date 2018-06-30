set(SRC_DIR ../VTK/)

ExternalProject_Add(VTK
  PREFIX                vtk
  GIT_REPOSITORY        https://github.com/Kitware/VTK.git
  GIT_TAG               v8.1.1
  
  CMAKE_ARGS            -DBUILD_SHARED_LIBS=ON
  
  CONFIGURE_COMMAND
      cmake -GNinja ${SRC_DIR}
      -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR}
      
  BUILD_COMMAND         ninja
  INSTALL_COMMAND       ninja install
)

# ExternalProject_Get_Property(Protobuf EXT_INSTALL_DIR)
message(STATUS "Install directory ${EXT_INSTALL_DIR}")
