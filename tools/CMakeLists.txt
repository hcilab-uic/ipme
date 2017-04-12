set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /home/harish/.local/opencv)
find_package(OpenCV REQUIRED CONFIG)
find_package(Boost REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(debug_tool)
add_subdirectory(signal_server)
