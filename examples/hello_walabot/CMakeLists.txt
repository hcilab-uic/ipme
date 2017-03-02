set(SRCS hello_walabot.cpp)

set(CMAKE_CXX_FLAGS "-std=c++14 -D__LINUX__")

add_executable(hello_walabot ${SRCS})

target_link_libraries(hello_walabot WalabotAPI)

