cmake_minimum_required(VERSION 3.18)

project(CMSWebPlugin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Gui Widgets WebSockets WebEngineWidgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Gui Widgets WebSockets WebEngineWidgets
  REQUIRED)

# find_package( Qt6 COMPONENTS Core REQUIRED) find_package( Qt6 COMPONENTS Gui
# REQUIRED) find_package( Qt6 COMPONENTS WebEngineWidgets REQUIRED)
# find_package( Qt6 COMPONENTS WebSockets REQUIRED)

include_directories(../../../CallGraph
                    ../../../BGMRPCModule/BGMRPC/BGMRPCClient
                    /usr/include/qt6)

qt_add_plugin(CMSWebPlugin CLASS_NAME CMSWebPlugin)
# qt_add_plugin(CMSWebPlugin STATIC CLASS_NAME CMSWebPlugin)

target_sources(
  CMSWebPlugin PRIVATE cmswebbrower.h cmswebbrower.cpp cmswebplugin.h
                       cmswebplugin.cpp cmswebtoolbox.h cmswebtoolbox.cpp)

target_include_directories(CMSWebPlugin PUBLIC ../../BGCMS)
target_link_libraries(
  CMSWebPlugin
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui
          Qt${QT_VERSION_MAJOR}::Widgets
          Qt${QT_VERSION_MAJOR}::WebEngineWidgets
          Qt${QT_VERSION_MAJOR}::WebSockets
          CallGraph BGMRPCClient qscintilla2_qt6)
