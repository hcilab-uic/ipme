find_package(Boost REQUIRED)

if(${BUILD_CV_TOOLS})
    include_directories(${OpenCV_INCLUDE_DIRS})
    add_subdirectory(camera_zoom)
endif()
