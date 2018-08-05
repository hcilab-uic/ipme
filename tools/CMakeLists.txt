include_directories(${ipmelib_DIR})

set(BUILD_WORKBENCH false CACHE BOOL "Build Proxemics visualization tool")

if(${BUILD_VISUALIZATION_TOOLS})
    if(${BUILD_SENSOR_WALABOT})
        add_subdirectory(debug_tool)
        add_subdirectory(signal_server)
    endif()
    add_subdirectory(visualization)
endif()

if(${BUILD_SENSOR_WALABOT})
    add_subdirectory(streaming_server)
endif()

add_subdirectory(data_collection)

if(${BUILD_WORKBENCH})
    add_subdirectory(workbench)
endif()

add_subdirectory(utilities)
