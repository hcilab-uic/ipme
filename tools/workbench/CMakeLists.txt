#cmake_minimum_required(VERSION 3.9)

#project(proxemics-wb)

find_package(Qt5 COMPONENTS Core Widgets Gui OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    color.cpp
    frame_model.cpp
    geometry.cpp
    main.cpp
    mainwindow.cpp
    point.cpp
    scene_object.cpp
    scene_widget.cpp

    color.h
    frame_model.h
    geometry.h
    mainwindow.h
    point.h
    scene_object.h
    scene_widget.h)

set(UI_SOURCES
    mainwindow.ui)

set(QT5_LIBRARIES Core Widgets Gui OpenGL)

set(TARGET proxemics-wb)

qt5_wrap_cpp(MainWindow_moc mainwindow.h)
qt5_wrap_ui(MainWindow_hdr ${UI_SOURCES})
add_library(MainWindow ${MainWindow_moc} ${MainWindow_hdr})
qt5_use_modules(MainWindow ${QT5_LIBRARIES})

add_executable(${TARGET} ${SOURCES} ${UI_GENERATED_HEADERS})
qt5_use_modules(${TARGET} ${QT5_LIBRARIES})
target_link_libraries(${TARGET} GL)
