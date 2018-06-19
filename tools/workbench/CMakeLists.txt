set(TARGET proxemics-wb)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    color.cpp
    frame_model.cpp
    geometry.cpp
    main.cpp
    sage_handler.cpp
    scene_object.cpp
    scene_widget.cpp

    color.h
    frame_model.h
    geometry.h
    scene_object.h
    scene_widget.h)

set(UI_SOURCES
    mainwindow.ui
    workbench_window.ui
    live_window.ui
)

include_directories(${OMICRON_SOURCE_PATH}/include)
include_directories(${Boost_INCLUDE_DIR})

add_library(workbench_ui SHARED
    mainwindow.cpp
    workbench_window.cpp
    live_window.cpp ${UI_SOURCES})
target_link_libraries(workbench_ui Qt5::Core Qt5::Widgets Qt5::Gui)

add_executable(${TARGET} ${SOURCES} resources.qrc)
target_link_libraries(${TARGET}
    workbench_ui ipme sensor GL ${OpenCV_LIBS}
)
