function(build_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ${ARGN})
endfunction(build_example)

build_example(dlib_metric_learning ipme)
