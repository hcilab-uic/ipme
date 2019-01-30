include_directories(${ipmelib_DIR})

set(BUILD_WORKBENCH false CACHE BOOL "Build Proxemics visualization tool")

add_subdirectory(data_collection)

if(${BUILD_WORKBENCH})
    add_subdirectory(workbench)
endif()

