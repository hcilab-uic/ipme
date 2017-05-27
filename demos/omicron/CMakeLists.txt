include_directories(${OMICRON_SOURCE_PATH}/include)

add_executable(client_connect client_connect.cpp)

target_link_libraries(client_connect ipme ${Boost_LIBRARIES} pthread)
