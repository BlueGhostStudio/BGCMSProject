#ifndef SNIPPETPLUGIN_H
#define SNIPPETPLUGIN_H

#include <bgmrpcclient.h>
#include <callgraph.h>
#include <cmsplugininterface.h>
#include <editorplugininterface.h>

#include <QtGui>

#include "SnippetPlugin_global.h"

struct t_snippet {
    QString label;
    QIcon icon;
    QString tex;
    int pos;
};

extern QString snippetsRelativePath;

class SNIPPETPLUGIN_EXPORT SnippetPlugin : public QObject,
                                           public EditorPluginInterface,
                                           public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.EditorPluginInterface" FILE
                          "SnippetPlugin.json")
    Q_INTERFACES(EditorPluginInterface CMSPluginInterface)

public:
    SnippetPlugin(QObject* parent = nullptr);

    QWidget* initialEditor(NS_BGMRPCClient::BGMRPCClient* bgmrpcClient,
                           QMainWindow* mainWin,
                           QsciScintilla* qsciEditor) override;
    t_ui initialCMS(NS_BGMRPCClient::BGMRPCClient*, QMainWindow* mainWin,
                    QObject* cmsWidget) override;

signals:
    void snippetsUpdated();

private slots:
    void editSnippets();
    // void onLocationChanged(int type, int cID, int cateID, int resID);

private:
    void loadSnippets();
    void loadSnippets(const QFileInfo& fileInfo);

private:
    // QVariantMap m_snippets;
    QMap<QString, QList<t_snippet> > m_snippets;
    QVariantMap m_resource;
};

#endif  // SNIPPETPLUGIN_H
