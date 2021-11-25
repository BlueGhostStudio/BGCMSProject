#ifndef SUBWINLIST_H
#define SUBWINLIST_H

#include <QListView>
#include <QMdiSubWindow>
#include <QStandardItem>
#include <QStandardItemModel>

#include "cmswidget.h"
class SubWinList : public QListView {
    Q_OBJECT
public:
    SubWinList(QWidget* parent = nullptr);

    // void initialConnect(CMSWidget* cms);

signals:
    void activeSubWin(int id);
    void closeSubWin(int id);

public slots:
    void addEditorItem(const QVariantMap& resource);
    void updateResItem(int res, bool m);

    void removeItem(int rID);

    void addPluginItem(int winID, const QString& title);
    void updatePluginItem(int winID, const QString& title);
    void updatePluginItemIcon(int winID, const QIcon& icon);

    void activedSubWinChanged(QMdiSubWindow* editorWin);

protected:
    void contextMenuEvent(QContextMenuEvent* event);

private:
    QStandardItem* findItem(int id);
    void updateResItem(QStandardItem* item);
    void updateResItem(QStandardItem* item, const QVariantMap& res);
    void updateResItem(QStandardItem* item, bool modified);

private:
    QStandardItemModel m_subWinListModel;
    QAction ui_close;
};

#endif  // SUBWINLIST_H
