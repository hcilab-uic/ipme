include_directories(../../sensor)

add_executable(debug_tool main.cpp)
target_link_libraries(debug_tool ${OpenCV_LIBS} sensor WalabotAPI)

