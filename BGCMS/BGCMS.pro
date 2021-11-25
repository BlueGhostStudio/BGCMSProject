QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += qscintilla2

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    categorydialog.cpp \
    cmsbrower.cpp \
    cmsfilterproxymodel.cpp \
    cmssubwin.cpp \
    cmswidget.cpp \
    collectiondialog.cpp \
    global.cpp \
    imgviewer.cpp \
    logindialog.cpp \
    main.cpp \
    bgcmsmainwin.cpp \
    pluginsubwin.cpp \
    resourcedialog.cpp \
    resourceeditor.cpp \
    ressubwin.cpp \
    ressubwinwidgetbase.cpp \
    subwinbase.cpp \
    subwinlist.cpp

HEADERS += \
    CMSPlugins/cmsplugininterface.h \
    CMSPlugins/editorplugininterface.h \
    bgcmsmainwin.h \
    categorydialog.h \
    cmsbrower.h \
    cmsfilterproxymodel.h \
    cmssubwin.h \
    cmswidget.h \
    collectiondialog.h \
    global.h \
    imgviewer.h \
    logindialog.h \
    pluginsubwin.h \
    resourcedialog.h \
    resourceeditor.h \
    ressubwin.h \
    ressubwinwidgetbase.h \
    subwinbase.h \
    subwinlist.h

FORMS += \
    bgcmsmainwin.ui \
    categorydialog.ui \
    cmswidget.ui \
    collectiondialog.ui \
    imgviewer.ui \
    logindialog.ui \
    resourcedialog.ui \
    resourceeditor.ui

TRANSLATIONS += \
    BGCMS_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#unix:!macx: LIBS += -L$$OUT_PWD/../BGMRPCModule/BGMRPC/BGMRPCClient/ -lBGMRPCClient

#INCLUDEPATH += $$PWD/../BGMRPCModule/BGMRPC/BGMRPCClient
#DEPENDPATH += $$PWD/../BGMRPCModule/BGMRPC/BGMRPCClient

#unix:!macx: LIBS += -L$$OUT_PWD/../BGMRPCModule/CallGraph/ -lCallGraph

#INCLUDEPATH += $$PWD/../BGMRPCModule/CallGraph
#DEPENDPATH += $$PWD/../BGMRPCModule/CallGraph

RESOURCES += \
    bgcms.qrc

RC_ICONS = appicon.ico

unix:!macx: LIBS += -L$$OUT_PWD/../../BGMRPCModule/BGMRPC/BGMRPCClient/ -lBGMRPCClient

INCLUDEPATH += $$PWD/../../BGMRPCModule/BGMRPC/BGMRPCClient
DEPENDPATH += $$PWD/../../BGMRPCModule/BGMRPC/BGMRPCClient


unix:!macx: LIBS += -L$$OUT_PWD/../../BGMRPCModule/CallGraph/ -lCallGraph

INCLUDEPATH += $$PWD/../../BGMRPCModule/CallGraph
DEPENDPATH += $$PWD/../../BGMRPCModule/CallGraph
