#ifndef CMSPLUGININTERFACE_H
#define CMSPLUGININTERFACE_H

#include <bgmrpcclient.h>

#include <QMainWindow>
#include <QMenu>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>
#include <functional>

class CMSPluginInterface {
public:
    struct t_ui {
        QWidget* toolboxWidget = nullptr;
        QWidget* pluginPage = nullptr;
        QAction* pluginAction = nullptr;
    };
    virtual t_ui initialCMS(NS_BGMRPCClient::BGMRPCClient* bgmrpcClient,
                            QMainWindow* mainWin, QObject* cmsWidget) = 0;
    std::function<void(QWidget* widget, bool deleteOnClose)> openPluginWin;
};

Q_DECLARE_INTERFACE(CMSPluginInterface, "org.bgstudio.BGCMS.CMSPluginInterface")

#endif  // CMSPLUGININTERFACE_H
