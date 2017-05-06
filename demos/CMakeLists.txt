find_package(Boost REQUIRED)

include_directories(${cdilib_DIR})

if(${BUILD_VISUALIZATION_TOOLS})
    include_directories(${OpenCV_INCLUDE_DIRS})
    include_directories(${VTK_INCLUDE_DIRS})

    function(build_viz_demo demo_name)
        add_executable(${demo_name} ${demo_name}.cpp)
        target_link_libraries(${demo_name} ${OpenCV_LIBS} ${VTK_LIBRARIES}
            boost_system pthread)
    endfunction(build_viz_demo)

    add_subdirectory(camera_zoom)
    add_subdirectory(vtk)
endif()
