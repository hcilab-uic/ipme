set(lib_SOURCES
    core/point.cpp
    data/person.cpp
    data/scene.cpp
    data/trackable_factory.cpp
    data/trackable_object.cpp
    io/json_parser.cpp
    io/parser.cpp
)

add_subdirectory(protobuf)
add_subdirectory(sensor)
add_subdirectory(utils)

add_library(ipme SHARED ${lib_SOURCES})
target_link_libraries(ipme sensor proto utils dlib::dlib)
target_include_directories(ipme PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
