cmake_minimum_required(VERSION 3.18)

project(SnippetPlugin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Gui Widgets WebSockets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Gui Widgets WebSockets
  REQUIRED)

include_directories(../../../CallGraph
                    ../../../BGMRPCModule/BGMRPC/BGMRPCClient /usr/include/qt6)

qt_add_plugin(SnippetPlugin CLASS_NAME SnippetPlugin)

target_sources(
  SnippetPlugin
  PRIVATE editsnippets.h
          editsnippets.cpp
          editsnippets.ui
          snippetplugin.h
          snippetplugin.cpp
          snippets.qrc
          snippetstoolbox.h
          snippetstoolbox.cpp
          snippetstoolbox.ui
          snippetwidget.h
          snippetwidget.cpp
          snippetwidget.ui)

target_include_directories(SnippetPlugin PUBLIC ../../BGCMS)
target_link_libraries(
  SnippetPlugin
  PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets
          Qt${QT_VERSION_MAJOR}::WebSockets CallGraph BGMRPCClient
          qscintilla2_qt6)
