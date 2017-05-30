include_directories(${ipmelib_DIR} ${CMAKE_SOURCE_DIR}/lib
    ${OMICRON_SOURCE_PATH}/include)

# FIXME: Recommended way is to build GTest along with the source
# https://github.com/google/googletest/blob/master/googletest/README.md
set(GTEST_LIBRARY ${GTEST_BUILD_DIR}/libgtest.so)
set(GTEST_MAIN_LIBRARY ${GTEST_BUILD_DIR}/libgtest_main.so)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

function(build_unittest unittest)
    add_executable(${unittest} ${unittest}.cpp)
    target_link_libraries(${unittest} ipme sensor ${GTEST_BOTH_LIBRARIES})
    add_test(AllTestsIn_${unittest} ${unittest})
endfunction(build_unittest)

build_unittest(vicon_listener_unittest)
