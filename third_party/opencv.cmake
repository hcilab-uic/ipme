set(SRC_DIR ../OpenCV/)
set(protobuf_build_dir
    "${CMAKE_CURRENT_BINARY_DIR}/protobuf/src/Protobuf-build/")

ExternalProject_Add(OpenCV
  PREFIX                opencv
  GIT_REPOSITORY        https://github.com/opencv/opencv.git
  GIT_TAG               4.0.1

  CMAKE_CACHE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DProtobuf_DIR:PATH=${protobuf_build_dir}
      -DCMAKE_INSTALL_PREFIX:PATH=${EXT_INSTALL_DIR}
      -DCMAKE_INSTALL_RPATH:PATH=${EXT_INSTALL_DIR}/lib
      -DCMAKE_PREFIX_PATH:PATH=${EXT_INSTALL_DIR}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DWITH_TBB:BOOL=ON
      -DWITH_OPENMP:BOOL=ON
      -DWITH_IPP:BOOL=ON
      -DWITH_NVCUVID:BOOL=ON
      -DWITH_VTK:BOOL=OFF
      -DWITH_WEBP:BOOL=OFF
      -DWITH_LAPACK:BOOL=OFF
      -DWITH_CUBLAS:BOOL=OFF
      -DWITH_CUFFT:BOOL=OFF
      -DWITH_CUDA:BOOL=OFF
      -DBUILD_PERF_TESTS:BOOL=OFF
      -DBUILD_TESTS:BOOL=OFF
      -DBUILD_PROTOBUF:BOOL=OFF
      -DBUILD_opencv_java:BOOL=OFF
      -DBUILD_opencv_python:BOOL=OFF
      -DBUILD_opencv_python2:BOOL=OFF
      -DBUILD_opencv_python3:BOOL=OFF
      -DBUILD_opencv_apps:BOOL=OFF
      -DBUILD_opencv_shape:BOOL=OFF
      -DBUILD_opencv_stitching:BOOL=OFF
      -DBUILD_opencv_superres:BOOL=OFF
      -DBUILD_opencv_ts:BOOL=OFF
      -DBUILD_opencv_cnn_3dobj:BOOL=OFF
      -DBUILD_opencv_viz:BOOL=OFF
      -DBUILD_opencv_world:BOOL=OFF
      -DBUILD_opencv_dnn:BOOL=OFF
      -DBUILD_opencv_ml:BOOL=OFF
      -DBUILD_opencv_flann:BOOL=OFF
      -DBUILD_opencv_photo:BOOL=OFF
)

