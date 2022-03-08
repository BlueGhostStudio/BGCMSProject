#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <bgmrpcclient.h>
#include <callgraph.h>

#include <QLabel>
#include <QMainWindow>
#include <QMenu>

#include "./ui_mainwindow.h"
#include "cmsapi.h"

// extern NS_BGMRPCClient::BGMRPCClient BGCMSClient;

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent*) override;

private slots:
    void connect2Host();
    void disconnect();

    void uiWorkStatus(bool working);

    void statusMessage(const QString& message, int timeOut = 2500);
    void changeAccount();

    void newBlankNode();

private:
    void initial();
    void initialPlugins();
    void uiConnectStatus(bool connected);
    void uiStatus(bool enabled);
    bool hasClip() const;
    QString token(bool ch = false);

private:
    QLabel* m_connectStatus;
    QPixmap m_connectedPixmap;
    QPixmap m_disconnectedPixmap;
    QMenu* m_newNodeMenu;

    QVariantMap m_callHistory;

    CMSApi m_api;
};
#endif  // MAINWINDOW_H
