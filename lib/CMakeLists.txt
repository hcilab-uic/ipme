# Artifact of QtCreator which currently happens to be my primary IDE:
# As a consequence of a stupid frustrating lack of feature, unless you
# (incorrectly) add a header file to a cmake target, it won't show up in
# 'Projects' view navigation window. So making a pseudo target here and
# adding the relevant header files

set(lib_HEADERS
    core/point.h
    data/person.h
    data/trackable_factory.h
    data/trackable_object.h
    data/vicon_point.h
    io/json_parser.h
    io/parser.h
    utils/config.h
    utils/omicron_config.h
    utils/logger.h
    utils/random.h utils/random.ipp
    utils/string_utils.h
    utils/utils.h
    data/point3d.h data/point3d.ipp
    visualization/polydata.h visualization/polydata.ipp
)

set(lib_SOURCES
    core/point.cpp
    data/person.cpp
    data/trackable_factory.cpp
    data/trackable_object.cpp
    io/json_parser.cpp
    io/parser.cpp
    utils/config.cpp
    utils/omicron_config.cpp
    utils/logger.cpp
    utils/string_utils.cpp
    utils/utils.cpp

    ${lib_HEADERS}
)

add_library(ipme ${lib_SOURCES} ${Boost_LIBRARIES})

add_subdirectory(sensor)

