cmake_minimum_required(VERSION 3.14)

project(CMSMedia LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Gui Widgets WebSockets Network
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Gui Widgets WebSockets Network
  REQUIRED)

include_directories(../../CallGraph ../../BGMRPCClient)

add_library(
  CMSMedia SHARED
  CMSMedia_global.h
  cmsmedia.cpp
  cmsmedia.h
  ../../BGCMS/cmsapi.h
  ../../BGCMS/cmsapi.cpp
  cmsmediaapi.h
  cmsmediaapi.cpp
  imageviewer.h
  imageviewer.cpp
  imageviewer.ui
  icons.qrc)

target_link_libraries(
  CMSMedia
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::WebSockets
          Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets CallGraph
          BGMRPCClient)

target_compile_definitions(CMSMedia PRIVATE CMSMEDIA_LIBRARY)
