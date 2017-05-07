set(SRCS hello_walabot.cpp)

include_directories(${cdilib_DIR})

add_executable(hello_walabot ${SRCS})

target_link_libraries(hello_walabot sensor WalabotAPI)
