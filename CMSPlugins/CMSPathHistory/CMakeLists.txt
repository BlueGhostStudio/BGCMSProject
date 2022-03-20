cmake_minimum_required(VERSION 3.14)

project(CMSPathHistory LANGUAGES CXX)

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

add_library(CMSPathHistory SHARED
  CMSPathHistory_global.h
  cmspathhistory.cpp
  cmspathhistory.h
)

target_link_libraries(
  CMSPathHistory
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::WebSockets
          Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets CallGraph
          BGMRPCClient)

target_compile_definitions(CMSPathHistory PRIVATE CMSPATHHISTORY_LIBRARY)

set_target_properties(
  CMSPathHistory
  PROPERTIES OUTPUT_NAME CMSPathHistory
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH ${CMSRPATH})

include(../../BGCMS.cmake)

install(TARGETS CMSPathHistory
                 LIBRARY DESTINATION ${DATALOCATION}/plugins)
