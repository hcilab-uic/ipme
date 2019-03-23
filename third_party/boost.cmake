set(SRC_DIR ../Boost/)

set(boost_rpath "${EXT_INSTALL_DIR}/lib")

set(b2_command "./b2")
set(bootstrap_command "./bootstrap.sh")

if(MSVC)
	set(b2_command "b2")
	set(bootstrap_command "bootstrap")
	message(STATUS "Bootstrapping for Microsoft platform b2 command=${b2_command}, bootstrap command=${bootstrap_command}")
endif(MSVC)

set(boost_build
        ${b2_command} -j8 install -d+2 --prefix=${EXT_INSTALL_DIR}
            --layout=tagged
			--with-python
	    link=shared
	    runtime-link=shared
	    dll-path=${boost_rpath}
	    threading=multi
		address-model=64
)

set(boost_url
  "https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.gz")

ExternalProject_Add(Boost
  PREFIX                boost
  URL                   ${boost_url}

  CMAKE_CACHE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DCMAKE_INSTALL_PREFIX:PATH=${EXT_INSTALL_DIR}
      -DCMAKE_INSTALL_RPATH:PATH=${EXT_INSTALL_DIR}/lib
      -DCMAKE_PREFIX_PATH:PATH=${EXT_INSTALL_DIR}


  CONFIGURE_COMMAND cd ${SRC_DIR} && pwd && ${bootstrap_command}

  BUILD_COMMAND
      cd ${SRC_DIR} && pwd && ${boost_build}

  INSTALL_COMMAND       ""
)
