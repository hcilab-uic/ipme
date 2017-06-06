include_directories(${OMICRON_SOURCE_PATH}/include)

function(build_vicon_demo demo)
    add_executable(${demo} ${demo}.cpp)
    target_link_libraries(${demo} ipme sensor ${VTK_LIBRARIES}
        ${Boost_LIBRARIES} pthread)
endfunction(build_vicon_demo)

if(${Boost_FOUND})
    build_vicon_demo(client_connect)
    build_vicon_demo(vicon_visualizer)
endif()
