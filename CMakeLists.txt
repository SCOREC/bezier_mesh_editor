# Generated from elasticnodes.pro.

cmake_minimum_required(VERSION 3.16)
project(bezier_mesh_editor LANGUAGES CXX)

set(CMAKE_C_COMPILER "/usr/bin/mpicc.mpich")
set(CMAKE_CXX_COMPILER "/usr/bin/mpicxx.mpich")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/widgets/graphicsview/elasticnodes")

#set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} /home/morteza/Sandbox/install/core/lib/cmake")
set(SCOREC_DIR "/home/morteza/Sandbox/install/core")
message(STATUS "scorec dir ${SCOREC_DIR}")
find_package(SCOREC 2.1.0 REQUIRED PATHS ${SCOREC_DIR})

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)

qt_add_executable(bezier_mesh_editor
    edge.cpp edge.h
    meshwidget.cpp meshwidget.h
    main.cpp
    node.cpp node.h
)
    #graphwidget.cpp graphwidget.h
target_include_directories(bezier_mesh_editor PUBLIC
    $<BUILD_INTERFACE:/usr/include/x86_64-linux-gnu/mpich>)
set_target_properties(bezier_mesh_editor PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(bezier_mesh_editor PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    SCOREC::core
)

install(TARGETS bezier_mesh_editor
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
