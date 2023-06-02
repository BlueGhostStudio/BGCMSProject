cmake_minimum_required(VERSION 3.14)

project(Todo LANGUAGES CXX)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Gui Widgets WebSockets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Gui Widgets WebSockets
  REQUIRED)

include_directories(../../CallGraph ../../BGMRPCClient)

add_library(Todo SHARED
  Todo_global.h
  todo.cpp
  todo.h
  todoitemdelegate.h todoitemdelegate.cpp
  todoeditform.h todoeditform.cpp todoeditform.ui
  todolistview.h todolistview.cpp todolistview.ui
  todoapi.h todoapi.cpp
  todosortmodel.h todosortmodel.cpp
  ../../BGCMS/cmsapi.h
  ../../BGCMS/cmsapi.cpp
)

target_link_libraries(Todo
    PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::WebSockets
            Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets CallGraph
            BGMRPCClient)

target_compile_definitions(Todo PRIVATE TODO_LIBRARY)

set_target_properties(
  Todo
  PROPERTIES OUTPUT_NAME Todo
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH ${CMSRPATH})

include(../../BGCMS.cmake)

install(TARGETS Todo
        LIBRARY DESTINATION ${DATALOCATION}/plugins)
