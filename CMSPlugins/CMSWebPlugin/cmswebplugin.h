#ifndef CMSWEBPLUGIN_H
#define CMSWEBPLUGIN_H

#include <bgmrpcclient.h>
#include <cmsplugininterface.h>
#include <editorplugininterface.h>

#include <QtGui>

#include "cmswebbrower.h"
#include "cmswebplugin_global.h"

class CMSWEBPLUGIN_EXPORT CMSWebPlugin : public QObject,
                                         public CMSPluginInterface,
                                         public EditorPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "CMSWebPlugin.json")
    Q_INTERFACES(CMSPluginInterface EditorPluginInterface)

public:
    CMSWebPlugin(QObject* parent = nullptr);

    QWidget* initialEditor(NS_BGMRPCClient::BGMRPCClient*, QMainWindow*,
                           QsciScintilla* qsciEditor) override;
    CMSPluginInterface::t_ui initialCMS(NS_BGMRPCClient::BGMRPCClient*,
                                        QMainWindow* mainWin,
                                        QObject* cmsWidget) override;
    void setResource(const QVariantMap& resource, QWidget* widget) override;

signals:
    void openCurrentResource(int rID);
    void tokenChanged(const QString& token);

private slots:
    void saveToken(const QString& token);

private:
    CMSWebBrower m_CMSWeb;
    QString m_token;
};

#endif  // CMSWEBPLUGIN_H
