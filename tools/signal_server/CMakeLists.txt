include_directories(../../sensor)
add_executable(signal_server main.cpp)
target_link_libraries(signal_server ${OpenCV_LIBS} sensor WalabotAPI
    boost_program_options pthread)
