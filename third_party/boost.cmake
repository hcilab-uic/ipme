set(SRC_DIR ../Boost/)


set(prefix_path ${EXT_INSTALL_DIR})
set(boost_rpath "${prefix_path}/lib")
set(b2_command "./b2")
set(bootstrap_command "./bootstrap.sh")

include (ProcessorCount)
ProcessorCount(N_CORES)

set(url_prefix "https://dl.bintray.com/boostorg/release")
set(major_version "1")
set(minor_version "68")
set(release_build "0")
set(release_tag "${major_version}.${minor_version}.${release_build}")
set(file_name "boost_${major_version}_${minor_version}_${release_build}.tar.gz")

if(MSVC)
    set(b2_command "b2")
    set(bootstrap_command "bootstrap")
    message(STATUS "Bootstrapping for Microsoft platform b2
        command=${b2_command}, bootstrap command=${bootstrap_command}")
endif(MSVC)

set(boost_build
    ${b2_command} -j${N_CORES} install -d+2 --prefix=${prefix_path}
        --layout=tagged
        link=shared
        runtime-link=shared
        dll-path=${boost_rpath}
        threading=multi
        address-model=64
)

set(boost_url "${url_prefix}/${release_tag}/source/${file_name}")

ExternalProject_Add(Boost
  PREFIX boost
  URL    ${boost_url}

  CMAKE_CACHE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DCMAKE_PREFIX_PATH:PATH=${prefix_path}
      -DCMAKE_INSTALL_PREFIX:PATH=${prefix_path}
      -DCMAKE_INSTALL_RPATH:PATH=${boost_rpath}

  CONFIGURE_COMMAND cd ${SRC_DIR} && pwd && ${bootstrap_command}

  BUILD_COMMAND
      cd ${SRC_DIR} && pwd && ${boost_build}

  INSTALL_COMMAND       ""
  INSTALL_DIR           "${prefix_path}"
)
