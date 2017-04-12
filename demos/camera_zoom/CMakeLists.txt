add_executable(camera_zoom camera_zoom.cpp)
target_link_libraries(camera_zoom ${OpenCV_LIBS} boost_system pthread)
