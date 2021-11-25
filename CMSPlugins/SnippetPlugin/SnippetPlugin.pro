QT += gui websockets

TEMPLATE = lib
DEFINES += SNIPPETPLUGIN_LIBRARY

CONFIG += c++11
CONFIG += qscintilla2 plugin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    editsnippets.cpp \
    snippetplugin.cpp \
    snippetstoolbox.cpp \
    snippetwidget.cpp

HEADERS += \
    SnippetPlugin_global.h \
    editsnippets.h \
    snippetplugin.h \
    snippetstoolbox.h \
    snippetwidget.h

INCLUDEPATH+=..

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    SnippetPlugin.json

FORMS += \
    editsnippets.ui \
    snippetstoolbox.ui \
    snippetwidget.ui

#unix:!macx: LIBS += -L$$OUT_PWD/../../../BGMRPCModule/BGMRPC/BGMRPCClient/ -lBGMRPCClient

#INCLUDEPATH += $$PWD/../../../BGMRPCModule/BGMRPC/BGMRPCClient
#DEPENDPATH += $$PWD/../../../BGMRPCModule/BGMRPC/BGMRPCClient


#unix:!macx: LIBS += -L$$OUT_PWD/../../../BGMRPCModule/CallGraph/ -lCallGraph

#INCLUDEPATH += $$PWD/../../../BGMRPCModule/CallGraph
#DEPENDPATH += $$PWD/../../../BGMRPCModule/CallGraph

RESOURCES += \
    snippets.qrc

unix:!macx: LIBS += -L$$OUT_PWD/../../../../BGMRPCModule/BGMRPC/BGMRPCClient/ -lBGMRPCClient

INCLUDEPATH += $$PWD/../../../../BGMRPCModule/BGMRPC/BGMRPCClient
DEPENDPATH += $$PWD/../../../../BGMRPCModule/BGMRPC/BGMRPCClient

unix:!macx: LIBS += -L$$OUT_PWD/../../../../BGMRPCModule/CallGraph/ -lCallGraph

INCLUDEPATH += $$PWD/../../../../BGMRPCModule/CallGraph
DEPENDPATH += $$PWD/../../../../BGMRPCModule/CallGraph
