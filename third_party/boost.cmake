set(SRC_DIR ../Boost/)

set(boost_rpath "${EXT_INSTALL_DIR}/lib")

set(boost_build
        ./b2 -j8 install -d+2 --prefix=${EXT_INSTALL_DIR}
            --layout=tagged
	    link=shared
	    runtime-link=shared
	    dll-path=${boost_rpath}
	    threading=multi
)

set(boost_url
  "https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz")


ExternalProject_Add(Boost
  PREFIX                boost
  URL                   ${boost_url}
  # GIT_REPOSITORY        https://github.com/boostorg/boost.git
  # GIT_TAG               boost-1.67.0

  CMAKE_CACHE_ARGS
      -DBUILD_SHARED_LIBS:BOOL=ON

  CONFIGURE_COMMAND    cd ${SRC_DIR} && pwd && ./bootstrap.sh
  BUILD_COMMAND
      cd ${SRC_DIR} && pwd && ${boost_build}


  INSTALL_COMMAND       ""
)
