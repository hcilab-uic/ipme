set(src_dir ../dlib/)

ExternalProject_Add(Dlib
    PREFIX         dlib
    GIT_REPOSITORY https://github.com/davisking/dlib.git
    GIT_TAG        v19.17

    CMAKE_CACHE_ARGS
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX:PATH=${EXT_INSTALL_DIR}
        -DCMAKE_PREFIX_PATH:PATH=${EXT_INSTALL_DIR}/lib
        -DBUILD_SHARED_LIBS:BOOL=ON
)
