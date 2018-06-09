set(TARGET proxemics-wb)

#find_package(Qt5 COMPONENTS Core Widgets Gui OpenGL REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    color.cpp
    frame_model.cpp
    geometry.cpp
    main.cpp
    scene_object.cpp
    scene_widget.cpp

    color.h
    frame_model.h
    geometry.h
    scene_object.h
    scene_widget.h)

set(UI_SOURCES
    mainwindow.ui)

#set(QT5_LIBRARIES Core Widgets Gui OpenGL)

#qt5_wrap_cpp(MainWindow_moc mainwindow.h)
#qt5_wrap_ui(MainWindow_hdr ${UI_SOURCES})
add_library(MainWindow mainwindow.cpp)
target_link_libraries(MainWindow Qt5::Core Qt5::Widgets Qt5::Gui)
#qt5_use_modules(MainWindow ${QT5_LIBRARIES})

add_executable(${TARGET} ${SOURCES} resources.qrc)
#qt5_use_modules(${TARGET} ${QT5_LIBRARIES})
target_link_libraries(${TARGET}
    MainWindow GL
    ${OpenCV_LIBS}
)
