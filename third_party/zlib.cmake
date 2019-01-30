set(SRC_DIR ../Boost/)

set(boost_rpath "${EXT_INSTALL_DIR}/lib")

set(zlib_build
    PREFIX             zlib
    GIT_REPOSITORY     https://github.com/madler/zlib.git
    GIT_TAG            v1.2.11

    SOURCE_DIR         zlib

    CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${EXT_INSTALL_DIR}
        -DCMAKE_INSTALL_RPATH:PATH=${EXT_INSTALL_DIR}/lib
        -DCMAKE_PREFIX_PATH:PATH=${EXT_INSTALL_DIR}
)
