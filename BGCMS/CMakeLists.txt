cmake_minimum_required(VERSION 3.5)

project(
  BGCMS
  VERSION 0.1
  LANGUAGES CXX)

include(../cmake/BGStudio.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Widgets LinguistTools
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Widgets LinguistTools
  REQUIRED)
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Gui Widgets WebSockets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Gui Widgets WebSockets
  REQUIRED)

set(TS_FILES BGCMS_zh_CN.ts)

#include_directories(../../CallGraph ../../BGMRPCModule/BGMRPC/BGMRPCClient)
include_directories(../CallGraph ../BGMRPCClient)

include(../BGCMS.cmake)

add_compile_definitions(DATALOCATION="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/BGStudio/BGCMS")

set(PROJECT_SOURCES
    main.cpp
    bgcms.qrc
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    cmsapi.h
    cmsapi.cpp
    cmsbrowserbase.h
    cmsbrowser.h
    cmsbrowser.cpp
    cmssortfiltermodel.h
    cmssortfiltermodel.cpp
    cmsdialog.h
    cmsdialog.cpp
    newnodedialog.h
    newnodedialog.cpp
    newnodedialog.ui
    preferencesdialog.h
    preferencesdialog.cpp
    preferencesdialog.ui
    ../CMSPlugins/cmsplugininterface.h
    ${TS_FILES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(BGCMS MANUAL_FINALIZATION ${PROJECT_SOURCES})
  # Define target properties for Android with Qt 6 as: set_property(TARGET BGCMS
  # APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  # ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
  # https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

  qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
  if(ANDROID)
    add_library(BGCMS SHARED ${PROJECT_SOURCES})
    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(BGCMS ${PROJECT_SOURCES})
  endif()

  qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(
  BGCMS
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::WebSockets
          Qt${QT_VERSION_MAJOR}::Gui CallGraph BGMRPCClient)

set_target_properties(
  BGCMS
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER cms.bgstudio.tk
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             OUTPUT_NAME BGCMS
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH ${CMSRPATH})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(BGCMS)
endif()

install(
  TARGETS BGCMS
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
    COMPONENT BGCMS)
