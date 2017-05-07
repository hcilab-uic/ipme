function(build_tool toolname)
    add_executable(${toolname} ${toolname}.cpp)
    target_link_libraries(${toolname} ${OpenCV_LIBS} ${VTK_LIBRARIES} sensor
        WalabotAPI boost_program_options boost_system pthread)
endfunction(build_tool)

build_tool(scene_dumper)
