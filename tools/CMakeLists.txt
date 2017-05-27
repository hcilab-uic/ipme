include_directories(${ipmelib_DIR})

if(${BUILD_VISUALIZATION_TOOLS})
    add_subdirectory(debug_tool)
    add_subdirectory(signal_server)
    add_subdirectory(visualization)
endif()

add_subdirectory(streaming_server)
