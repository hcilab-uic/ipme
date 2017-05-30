include_directories(${CMAKE_SOURCE_DIR}/lib
    ${OMICRON_SOURCE_PATH}/include)

get_directory_property(incdir INCLUDE_DIRECTORIES)

set(SRCS
    sensor.cpp
    vicon_listener.cpp
    vicon_eventlogger.cpp
    walabot.cpp

    # Adding these headers so that QtCreator IDE can show them in projects view
    # I know this is dumb, but there is no other way to do it
    receiver.h
    receiver3d.h receiver3d.ipp
    sensor.h
    vicon_datahandler.h
    vicon_eventlogger.h
    vicon_listener.h
    walabot.h
    walabot3d.h walabot3d.ipp
    walabot_receiver_writer.h walabot_receiver_writer.ipp
)

add_library(sensor ${SRCS})

target_include_directories(sensor PUBLIC
    $<BUILD_INTERFACE:${OMICRON_SOURCE_PATH}/include>
)
