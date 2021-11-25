#ifndef EDITORPLUGININTERFACE_H
#define EDITORPLUGININTERFACE_H

#include <Qsci/qsciscintilla.h>
#include <bgmrpcclient.h>

#include <QMainWindow>

class EditorPluginInterface {
public:
    virtual QWidget* initialEditor(NS_BGMRPCClient::BGMRPCClient* bgmrpcClient,
                                   QMainWindow* mainWin,
                                   QsciScintilla* qsciEditor) = 0;
    virtual void setResource(const QVariantMap&, QWidget*){};
};

Q_DECLARE_INTERFACE(EditorPluginInterface,
                    "org.bgstudio.BGCMS.EditorPluginInterface")

#endif  // EDITORPLUGININTERFACE_H
