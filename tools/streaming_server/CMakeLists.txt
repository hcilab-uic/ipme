include_directories(../../sensor)
add_executable(streaming_server main.cpp)
target_link_libraries(streaming_server ${OpenCV_LIBS} sensor WalabotAPI
    boost_program_options boost_system pthread)
