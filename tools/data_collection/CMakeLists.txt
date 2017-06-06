include_directories(${OMICRON_SOURCE_PATH}/include)

function(build_data_collection_tool dctool)
    add_executable(${dctool} ${dctool}.cpp)
    target_link_libraries(${dctool} ipme sensor)
endfunction(build_data_collection_tool)

if(${BUILD_VICON_CODE})
    build_vicon_demo(vicon_record_csv)
endif()

