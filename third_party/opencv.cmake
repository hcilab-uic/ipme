set(SRC_DIR ../OpenCV/)

ExternalProject_Add(OpenCV
  PREFIX                opencv
  GIT_REPOSITORY        https://github.com/opencv/opencv.git
  GIT_TAG               3.4.1
  
  CMAKE_ARGS            -DBUILD_SHARED_LIBS=ON
  CMAKE_CACHE_ARGS
    -DWITH_VTK:BOOL=OFF
    -DWITH_WEBP:BOOL=OFF
    -DWITH_LAPACK:BOOL=OFF
    -DBUILD_PERF_TESTS:BOOL=OFF
    -DBUILD_TESTS:BOOL=OFF        
    -DBUILD_PROTOBUF:BOOL=OFF
    -DProtobuf_DIR:PATH=${EXT_INSTALL_DIR}
  
  CONFIGURE_COMMAND
    cmake -GNinja ${SRC_DIR}
    -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR}
    -DVTK_DIR=${EXT_INSTALL_DIR}/lib/cmake/vtk-8.1
    
  BUILD_COMMAND         ninja
  INSTALL_COMMAND       ninja install
)

# ExternalProject_Get_Property(Protobuf EXT_INSTALL_DIR)
message(STATUS "Install directory ${EXT_INSTALL_DIR}")
