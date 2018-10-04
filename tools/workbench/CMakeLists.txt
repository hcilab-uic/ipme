set(TARGET proxemics-wb)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt53DCore REQUIRED)
find_package(Qt53DExtras REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    color.cpp
    config.cpp
    frame.cpp
    frame_collection.cpp
    frame_model.cpp
    geometry.cpp
    main.cpp
    sage/create_app_window_handler.cpp
    sage/default_sage_message_handler.cpp
    sage/sage_element.cpp
    sage/sage_element_container.cpp
    sage/update_item_order_handler.cpp
    sage_handler.cpp
    sage_session.cpp
    scene_object.cpp
    scene_widget.cpp
    scene_modifier.cpp

    color.h
    frame_model.h
    geometry.h
    scene_object.h
    scene_widget.h)

set(UI_SOURCES
    mainwindow.ui
    visualization_window.ui
    workbench_window.ui
    live_window.ui
    video_window.ui
)

include_directories(${OMICRON_SOURCE_PATH}/include)
include_directories(${Boost_INCLUDE_DIR})

add_library(workbench_ui SHARED
    mainwindow.cpp
    visualization_window.cpp
    workbench_window.cpp
    live_window.cpp
    video_window.cpp ${UI_SOURCES})

target_link_libraries(workbench_ui ipme Qt5::Core Qt5::Widgets Qt5::Gui
    Qt5::3DCore Qt5::3DExtras Qt5::3DRender Qt5::3DInput)

add_executable(${TARGET} ${SOURCES} resources.qrc)
target_link_libraries(${TARGET}
    workbench_ui ipme sensor GL ${OpenCV_LIBS}
)
