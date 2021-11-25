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
  COMPONENTS Gui Widgets WebSockets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Gui Widgets WebSockets
  REQUIRED)

include_directories(../../CallGraph /usr/include/qt6
                    ../../BGMRPCModule/BGMRPC/BGMRPCClient)

set(PROJECT_SOURCES
    main.cpp
    bgcms.qrc
    bgcmsmainwin.h
    bgcmsmainwin.cpp
    bgcmsmainwin.ui
    categorydialog.h
    categorydialog.cpp
    categorydialog.ui
    cmsbrower.h
    cmsbrower.cpp
    cmsfilterproxymodel.h
    cmsfilterproxymodel.cpp
    cmssubwin.h
    cmssubwin.cpp
    cmswidget.h
    cmswidget.cpp
    cmswidget.ui
    collectiondialog.h
    collectiondialog.cpp
    collectiondialog.ui
    global.h
    global.cpp
    imgviewer.h
    imgviewer.cpp
    imgviewer.ui
    logindialog.h
    logindialog.cpp
    logindialog.ui
    pluginsubwin.h
    pluginsubwin.cpp
    resourcedialog.h
    resourcedialog.cpp
    resourcedialog.ui
    resourceeditor.h
    resourceeditor.cpp
    resourceeditor.ui
    ressubwin.h
    ressubwin.cpp
    ressubwinwidgetbase.h
    ressubwinwidgetbase.cpp
    subwinbase.h
    subwinbase.cpp
    subwinlist.h
    subwinlist.cpp
    cmsplugininterface.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(BGCMS MANUAL_FINALIZATION ${PROJECT_SOURCES})
  # Define target properties for Android with Qt 6 as: set_property(TARGET BGCMS
  # APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  # ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
  # https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
  if(ANDROID)
    add_library(BGCMS SHARED ${PROJECT_SOURCES})
    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(BGCMS ${PROJECT_SOURCES})
  endif()
endif()

target_link_libraries(
  BGCMS
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::WebSockets
          Qt${QT_VERSION_MAJOR}::Gui CallGraph BGMRPCClient qscintilla2_qt6)

set_target_properties(
  BGCMS
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(BGCMS)
endif()

install(TARGETS BGMRPCCtrl
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
