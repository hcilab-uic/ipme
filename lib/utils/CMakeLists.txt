set(HDRS
    config.h
    omicron_config.h
    logger.h
    random.h random.ipp
    string_utils.h
    utils.h
)

set(SRCS
    config.cpp
    json.cpp
    omicron_config.cpp
    logger.cpp
    string_utils.cpp
    utils.cpp

    ${HDRS}
)

add_library(utils SHARED ${SRCS})
target_link_libraries(utils Boost::system Boost::log Boost::date_time
    Boost::log_setup Boost::thread Boost::program_options Boost::filesystem
    stdc++fs)
