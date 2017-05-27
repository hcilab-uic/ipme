function(build_tool toolname)
    add_executable(${toolname} ${toolname}.cpp)
    target_link_libraries(${toolname} ${OpenCV_LIBS} ${VTK_LIBRARIES} sensor
        ipme WalabotAPI ${Boost_LIBRARIES} pthread)
endfunction(build_tool)

build_tool(scene_dumper)
