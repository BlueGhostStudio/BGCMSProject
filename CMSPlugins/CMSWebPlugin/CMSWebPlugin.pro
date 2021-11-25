QT += gui webenginewidgets websockets

TEMPLATE = lib
DEFINES += CMSWEBPLUGIN_LIBRARY

CONFIG += c++11 plugin qscintilla2

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cmswebbrower.cpp \
    cmswebplugin.cpp \
    cmswebtoolbox.cpp

HEADERS += \
    cmswebbrower.h \
    cmswebplugin.h \
    cmswebplugin_global.h \
    cmswebtoolbox.h

INCLUDEPATH+=..

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    CMSWebPlugin.json

#unix:!macx: LIBS += -L$$OUT_PWD/../../../BGMRPCModule/BGMRPC/BGMRPCClient/ -lBGMRPCClient

#INCLUDEPATH += $$PWD/../../../BGMRPCModule/BGMRPC/BGMRPCClient
#DEPENDPATH += $$PWD/../../../BGMRPCModule/BGMRPC/BGMRPCClient


#unix:!macx: LIBS += -L$$OUT_PWD/../../../BGMRPCModule/CallGraph/ -lCallGraph

#INCLUDEPATH += $$PWD/../../../BGMRPCModule/CallGraph
#DEPENDPATH += $$PWD/../../../BGMRPCModule/CallGraph


FORMS += \
    cmswebtoolbox.ui

unix:!macx: LIBS += -L$$OUT_PWD/../../../../BGMRPCModule/BGMRPC/BGMRPCClient/ -lBGMRPCClient

INCLUDEPATH += $$PWD/../../../../BGMRPCModule/BGMRPC/BGMRPCClient
DEPENDPATH += $$PWD/../../../../BGMRPCModule/BGMRPC/BGMRPCClient

unix:!macx: LIBS += -L$$OUT_PWD/../../../../BGMRPCModule/CallGraph/ -lCallGraph

INCLUDEPATH += $$PWD/../../../../BGMRPCModule/CallGraph
DEPENDPATH += $$PWD/../../../../BGMRPCModule/CallGraph
