#ifndef BGCMSMAINWIN_H
#define BGCMSMAINWIN_H

#include <cmswidget.h>
#include <editorplugininterface.h>

#include <QActionGroup>
#include <QMainWindow>
#include <QMdiSubWindow>

#include "cmssubwin.h"
#include "ui_bgcmsmainwin.h"

class BGCMSMainWin : public QMainWindow, private Ui::BGCMSMainWin {
    Q_OBJECT

public:
    BGCMSMainWin(QWidget* parent = nullptr);
    ~BGCMSMainWin();

signals:
    void editorOpened(const QVariantMap& resource);
    void subWinClosed(int winID);
    void pluginSubWinOpened(int winID, const QString& title);
    void logined(const QString& token);

private slots:
    // public slots:
    // void login(const QString& token);
    void join();

    void changeViewMode(const QAction* action);
    void connectToCMS();
    void disconnectFromCMS();
    void openResource(const QVariantMap& resource);
    void openPluginWin(QWidget* widget, bool = true);
    void activeSubWin(int id);
    void closeSubWin(int id);
    void closeAllSubWin();

private:
    void initialPlugins();
    SubWinBase* findSubWin(int id);

private:
    CMSSubWin ui_cmsSubWin;

    QActionGroup m_viewMode;

    QList<QObject*> m_editorPlugins;

    QString m_token;
};
#endif  // BGCMSMAINWIN_H
