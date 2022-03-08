cmake_minimum_required(VERSION 3.14)

project(BrowserHeaderView LANGUAGES CXX)

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

include_directories(../../../CallGraph
                    ../../../BGMRPCModule/BGMRPC/BGMRPCClient)

add_library(
  BrowserHeaderView SHARED
  BrowserHeaderView_global.h browserheaderview.cpp browserheaderview.h
  ../../BGCMS/cmsapi.h ../../BGCMS/cmsapi.cpp)

target_link_libraries(
  BrowserHeaderView
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::WebSockets
          Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets CallGraph
          BGMRPCClient)

target_compile_definitions(BrowserHeaderView PRIVATE BROWSERHEADERVIEW_LIBRARY)
