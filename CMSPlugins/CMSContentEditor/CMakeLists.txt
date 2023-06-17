cmake_minimum_required(VERSION 3.14)

project(CMSContentEditor LANGUAGES CXX)

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

include_directories(../../CallGraph ../../BGMRPCClient /usr/include/qt6)

add_library(
  CMSContentEditor SHARED
  CMSContentEditor_global.h
  cmscontenteditor.cpp
  cmscontenteditor.h
  editorwindow.h
  editorwindow.cpp
  editorwindow.ui
  newnodedlg.h
  newnodedlg.cpp
  newnodedlg.ui
  icons.qrc
  ../../BGCMS/cmsdialog.h
  ../../BGCMS/cmsdialog.cpp
  ../../BGCMS/cmsapi.h
  ../../BGCMS/cmsapi.cpp)

target_link_libraries(
  CMSContentEditor
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
          Qt${QT_VERSION_MAJOR}::WebSockets
          Qt${QT_VERSION_MAJOR}::Gui
          Qt${QT_VERSION_MAJOR}::Widgets
          CallGraph
          BGMRPCClient
          qscintilla2_qt6)

target_compile_definitions(CMSContentEditor PRIVATE CMSCONTENTEDITOR_LIBRARY)

set_target_properties(
  CMSContentEditor
  PROPERTIES OUTPUT_NAME CMSContentEditor
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH ${CMSRPATH})

include(../../BGCMS.cmake)

install(TARGETS CMSContentEditor
        LIBRARY DESTINATION ${CMSDATALOCATION}/plugins)
