function(build_tool toolname)
    add_executable(${toolname} ${toolname}.cpp)
    target_link_libraries(${toolname} ${OpenCV_LIBS} ${VTK_LIBRARIES} ${Boost_LIBRARIES} pthread)
endfunction(build_tool)

if(${BUILD_SENSOR_WALABOT})
    build_tool(scene_dumper)
endif()

if(${BUILD_VISUALIZATION_TOOLS})
    build_tool(csvtovtp)
endif()