include_directories(${CMAKE_SOURCE_DIR}/lib
    ${OMICRON_SOURCE_PATH}/include)

get_directory_property(incdir INCLUDE_DIRECTORIES)

set(SRCS
    sensor.cpp
)

if(${BUILD_MOCAP_CODE})
    list(APPEND SRCS
        vicon_csvwriter.h vicon_csvwriter.cpp
        vicon_datahandler.h
        vicon_eventlogger.h vicon_eventlogger.cpp
        vicon_listener.h vicon_listener.cpp
        vrpn_handler.h vrpn_handler.cpp
    )

    if(${BUILD_VISUALIZATION_TOOLS})
        list(APPEND SRCS vicon_3dwriter.h vicon_3dwriter.cpp)
    endif()
endif()

add_library(sensor SHARED ${SRCS})

target_include_directories(sensor PUBLIC
    $<BUILD_INTERFACE:${OMICRON_SOURCE_PATH}/include>
)

target_link_libraries(sensor proto utils)
