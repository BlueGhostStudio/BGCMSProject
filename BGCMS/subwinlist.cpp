#include "subwinlist.h"

#include <QtDebug>

#include "imgviewer.h"
#include "resourceeditor.h"
#include "ressubwin.h"

SubWinList::SubWinList(QWidget* parent)
    : QListView(parent), ui_close(tr("Close")) {
    setModel(&m_subWinListModel);

    QStandardItem* homeItem =
        new QStandardItem(QIcon(":/imgs/home.png"), tr("CMS Home"));
    homeItem->setData(-1);
    m_subWinListModel.appendRow(homeItem);

    QObject::connect(
        this, &SubWinList::clicked, this, [=](const QModelIndex& index) {
            emit activeSubWin(index.data(Qt::UserRole + 1).toInt());
        });
    QObject::connect(
        &bgmrpcClient, &NS_BGMRPCClient::BGMRPCClient::onRemoteSignal, this,
        [=](const QString& obj, const QString& sig, const QJsonArray& args) {
            if (obj == "CMS" && sig == "updatedResource") {
                QVariantList argList = args.toVariantList();
                if (argList.count() > 0) {
                    QVariantMap vArg0Map = argList[0].toMap();
                    QStandardItem* item = findItem(vArg0Map["id"].toUInt());
                    updateResItem(item, vArg0Map);
                    /*if (item) {
                        item->setText(vArg0Map["title"].toString());
                        item->setIcon(resIcon(vArg0Map["type"].toString()));
                    }*/
                }
            }
        });
    QObject::connect(&ui_close, &QAction::triggered, this, [=]() {
        QStandardItem* item = m_subWinListModel.item(currentIndex().row());
        emit closeSubWin(item->data().toInt());
    });
}

/*void
SubWinList::initialConnect(CMSWidget* cms) {
    QObject::connect(
        cms, &CMSWidget::resourceUpdated, this,
        [=](const QVariantMap& resource) {
            QStandardItem* item = findItem(resource["id"].toUInt());
            if (item) {
                item->setText(resource["title"].toString());
                item->setIcon(resIcon(resource["type"].toString()));
            }
        });
}*/

void
SubWinList::addEditorItem(const QVariantMap& resource) {
    /*QStandardItem* item = new QStandardItem(
        resIcon(resource["type"].toString()), resource["title"].toString());
    item->setData(resource["id"]);
    item->setData(false, Qt::UserRole + 2);*/
    QStandardItem* item = new QStandardItem;
    updateResItem(item, resource);
    m_subWinListModel.appendRow(item);
    setCurrentIndex(item->index());
}

void
SubWinList::updateResItem(int res, bool m) {
    QStandardItem* item = findItem(res);
    updateResItem(item, m);
}

void
SubWinList::removeItem(int rID) {
    QStandardItem* item = findItem(rID);
    if (item) m_subWinListModel.removeRow(item->row());
}

void
SubWinList::addPluginItem(int winID, const QString& title) {
    QStandardItem* item = new QStandardItem;
    item->setData(winID);
    item->setText(title);
    m_subWinListModel.appendRow(item);
    setCurrentIndex(item->index());
}

void
SubWinList::updatePluginItem(int winID, const QString& title) {
    QStandardItem* item = findItem(winID);
    if (item) item->setText(title);
}

void
SubWinList::updatePluginItemIcon(int winID, const QIcon& icon) {
    QStandardItem* item = findItem(winID);
    if (item) item->setIcon(icon);
}

void
SubWinList::activedSubWinChanged(QMdiSubWindow* editorWin) {
    if (!editorWin) return;

    int id = -1;
    /*ResSubWinWidgetBase* subWinWidget =
        qobject_cast<ResSubWinWidgetBase*>(editorWin->widget());
    if (subWinWidget) id = subWinWidget->resID();*/
    // ResSubWin* resSubWin = qobject_cast<ResSubWin*>(editorWin);
    SubWinBase* subWin = qobject_cast<SubWinBase*>(editorWin);
    if (subWin) id = subWin->winID();

    QStandardItem* item = findItem(id);
    if (item) setCurrentIndex(item->index());
}

void
SubWinList::contextMenuEvent(QContextMenuEvent* event) {
    if (indexAt(event->pos()).isValid()) {
        QMenu::exec({ &ui_close }, event->globalPos());
    }
}

QStandardItem*
SubWinList::findItem(int id) {
    QStandardItem* item;
    bool found = false;
    for (int i = 0; i < m_subWinListModel.rowCount(); i++) {
        item = m_subWinListModel.item(i);
        if (item->data() == id) {
            found = true;
            break;
        }
    }

    return found ? item : nullptr;
}

void
SubWinList::updateResItem(QStandardItem* item) {
    if (item) {
        QString title = item->data(Qt::UserRole + 2).toString();
        bool m = item->data(Qt::UserRole + 3).toBool();
        item->setText(title + (m ? "*" : ""));
    }
}

void
SubWinList::updateResItem(QStandardItem* item, const QVariantMap& res) {
    if (item) {
        item->setIcon(resIcon(res["type"].toString()));
        item->setData(res["id"]);
        item->setData(res["title"], Qt::UserRole + 2);
        updateResItem(item);
    }
}

void
SubWinList::updateResItem(QStandardItem* item, bool modified) {
    if (item) {
        item->setData(modified, Qt::UserRole + 3);
        updateResItem(item);
    }
}
