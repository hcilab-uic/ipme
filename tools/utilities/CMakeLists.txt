function(build_utility name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ipme ${ARGN})
endfunction(build_utility)

build_utility(convertpb)
