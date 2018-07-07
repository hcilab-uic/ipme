set(SRC_DIR ../VTK/)

find_package(ZLIB REQUIRED)

ExternalProject_Add(VTK
  PREFIX                vtk
  GIT_REPOSITORY        https://github.com/Kitware/VTK.git
  GIT_TAG               v8.1.1

  CMAKE_CACHE_ARGS
      -DCMAKE_SHARED_LIBS:BOOL=ON
      -DCMAKE_INSTALL_PREFIX:PATH=${EXT_INSTALL_DIR}
      -DVTK_USE_SYSTEM_ZLIB:BOOL=ON
      -DCMAKE_BUILD_TESTING:BOOL=OFF
      -DCMAKE_BUILD_EXAMPLES:BOOL=OFF

  CONFIGURE_COMMAND
      cmake ${SRC_DIR}
      -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR}
)

# ExternalProject_Get_Property(Protobuf EXT_INSTALL_DIR)
message(STATUS "Install directory ${EXT_INSTALL_DIR}")
