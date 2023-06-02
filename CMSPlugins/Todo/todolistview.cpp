/*****************************************************************************
 * todolistview.cpp
 *
 * Created: 2022/4/15 2022 by blueghost
 *
 * Copyright 2022 blueghost. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#include "todolistview.h"

#include <QStandardItem>
#include <QStringListModel>

#include "todoeditform.h"
#include "todoitemdelegate.h"

TodoListView::TodoListView(TodoApi* api, QWidget* parent)
    : m_api(api), QWidget(parent) {
    setupUi(this);

    TodoItemDelegate* itemDelegate = new TodoItemDelegate(this);

    ui_lvTodo->setItemDelegate(itemDelegate);

    QObject::connect(itemDelegate, &TodoItemDelegate::removeTodo, this,
                     [=](const QModelIndex& index) {
                         m_api->remove(
                             index.data(Qt::UserRole + 1).toMap()["id"].toInt(),
                             nullptr, QString(), QString());
                     });
    QObject::connect(
        itemDelegate, &TodoItemDelegate::updateTodo, this,
        [=](const QModelIndex& index, const QString& field,
            const QVariant& data) {
            int id = index.data(Qt::UserRole + 1).toMap()["id"].toInt();

            if (field == "state")
                m_api->state(id, data, nullptr, QString(), QString());
            else if (field == "PRI")
                m_api->priority(id, data, nullptr, QString(), QString());
            else if (field == "label")
                m_api->label(id, data, nullptr, QString(), QString());
            else if (field == "content")
                m_api->content(id, data, nullptr, QString(), QString());
        });
    QObject::connect(itemDelegate, &TodoItemDelegate::closeEditor, this, [=]() {
        m_sortModel.sort(0);
        QModelIndex curIndex = ui_lvTodo->currentIndex();
        if (curIndex.isValid())
            ui_lvTodo->scrollTo(curIndex, QAbstractItemView::PositionAtTop);
        ui_lvTodo->clearSelection();
    });

    m_sortModel.setSourceModel(&m_todoModel);
    m_sortModel.setDynamicSortFilter(false);
    // m_sortModel.sort(0);
    ui_lvTodo->setModel(&m_sortModel);

    QObject::connect(m_api, &TodoApi::todoCreated, this,
                     [=](const QString& lid, const QVariantMap& todo) {
                         if (m_lID == lid) addTodoItem(todo);
                     });
    QObject::connect(m_api, &TodoApi::contentUpdated, this,
                     [=](int id, const QString& content) {
                         QStandardItem* item = findItem(id);
                         if (item) {
                             QVariantMap data = item->data().toMap();
                             data["content"] = content;
                             item->setData(data);
                             formatViewItem(item, data);
                         }
                     });

    QObject::connect(m_api, &TodoApi::priorityUpdated, this,
                     [=](int id, int priority) {
                         QStandardItem* item = findItem(id);
                         if (item) {
                             QVariantMap data = item->data().toMap();
                             data["PRI"] = priority;
                             item->setData(data);
                             formatViewItem(item, data);
                         }
                     });
    QObject::connect(m_api, &TodoApi::stateUpdated, this,
                     [=](int id, int state) {
                         QStandardItem* item = findItem(id);
                         if (item) {
                             QVariantMap data = item->data().toMap();
                             data["state"] = state;
                             item->setData(data);
                             formatViewItem(item, data);
                         }
                     });
    QObject::connect(m_api, &TodoApi::labelUpdated, this,
                     [=](int id, const QString& label) {
                         QStandardItem* item = findItem(id);
                         if (item) {
                             QVariantMap data = item->data().toMap();
                             data["label"] = label;
                             item->setData(data);
                             formatViewItem(item, data);
                         }
                     });
    QObject::connect(m_api, &TodoApi::removed, this, [=](int id) {
        QStandardItem* item = findItem(id);
        if (item) {
            m_todoModel.removeRow(item->row());
        }
    });
    QObject::connect(m_api, &TodoApi::allCompletedRemoved, this,
                     [=](const QString& lid, const QList<int>& ids) {
                         qDebug() << ids;
                         if (lid == m_lID) {
                             foreach (int id, ids) {
                                 QStandardItem* item = findItem(id);
                                 if (item) m_todoModel.removeRow(item->row());
                             }
                         }
                     });
}

void
TodoListView::load(const QVariant& node) {
    QVariantMap mNode = node.toMap();
    m_todoModel.clear();

    CallGraph::start("todoList", this)
        ->nodes("todoList",
                [=](CallGraph* cg, const QVariant&) {
                    if (mNode["type"].toString() == 'D')
                        cg->to("loadTodoList", "cms_" + mNode["id"].toString());
                    else if (mNode["contentType"].toString() == "todo")
                        cg->to("openTodoListNode", mNode["id"]);
                    else
                        cg->to("error");
                })
        ->nodes("loadTodoList",
                [=](CallGraph* cg, const QVariant& lID) {
                    m_lID = lID.toString();
                    m_api->list(lID.toString(), cg, "loadListView", "error");
                })
        ->nodes("loadListView",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantList list = data.toList();
                    qDebug() << list;
                    foreach (QVariant vTodoData, list) {
                        addTodoItem(vTodoData.toMap());
                    }
                    setEnabled(true);
                    m_sortModel.sort(0);
                    ui_lvTodo->scrollToTop();
                    cg->toFinal();
                })
        ->nodes("openTodoListNode",
                [=](CallGraph* cg, const QVariant& id) {
                    m_api->cmsApi()->node(id, cg, "getTodoListID", "error");
                })
        ->nodes("getTodoListID",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantMap node = data.toMap();
                    QString lid = node["content"].toString();
                    lid.remove(QRegularExpression("^res:"));
                    cg->to("loadTodoList", lid);
                })
        ->nodes("error",
                [=](CallGraph* cg, const QVariant&) {
                    setEnabled(false);
                    m_lID.clear();
                    cg->toFinal();
                })
        ->exec();
}

void
TodoListView::addTodo() {
    //    qDebug() << m_lID;
    m_api->newTodo(m_lID, nullptr, QString(), QString());
}

void
TodoListView::removeAllCompleted() {
    m_api->removeAllCompleted(m_lID, nullptr, QString(), QString());
}

void
TodoListView::addTodoItem(const QVariantMap& todo) {
    QStandardItem* item = new QStandardItem();

    item->setData(todo, Qt::UserRole + 1);
    formatViewItem(item, todo);
    m_todoModel.appendRow(item);
    m_sortModel.sort(0);

    QModelIndex scrToIndex = m_sortModel.mapFromSource(item->index());
    qDebug() << "new item index" << scrToIndex;
    ui_lvTodo->scrollTo(scrToIndex, QAbstractItemView::PositionAtTop);
}

void
TodoListView::formatViewItem(QStandardItem* item,
                             const QVariantMap& mTodoData) const {
    int state = mTodoData["state"].toInt();
    QFont font;
    font.setStrikeOut(state > 0);
    item->setData(font, Qt::FontRole);

    QColor color;
    bool ok;
    int nPRI = mTodoData["PRI"].toInt(&ok);
    if (!ok) nPRI = 2;
    if (nPRI == 2)
        item->setData(QVariant(), Qt::ForegroundRole);
    else {
        bool light = qApp->palette().base().color().lightnessF() > 0.5;
        if (nPRI == 1)
            color.setNamedColor(light ? "#be9501" : "#edc11e");
        else if (nPRI == 0)
            color.setNamedColor(light ? "#9e1e2b" : "#da4453");

        item->setData(color, Qt::ForegroundRole);
    }

    // set Content;
    QString label = mTodoData["label"].toString();
    if (!label.isEmpty()) label = "[" + label + "]";

    QString stateText;
    if (state == 1) stateText = "[" + tr("COMPLETED") + "]";
    if (state == 2)
        stateText = "[" + tr("OBSOLETE") + "]";
    else if (state == 3)
        stateText = "[" + tr("EXPIRED") + "]";

    item->setData(
        stateText + label + mTodoData["content"].toString() +
            QDateTime::fromMSecsSinceEpoch(mTodoData["date"].toLongLong())
                .toString("(M/d/yyyy)"),
        Qt::DisplayRole);
}

QStandardItem*
TodoListView::findItem(int id) {
    for (int i = 0; i < m_todoModel.rowCount(); i++) {
        QStandardItem* item = m_todoModel.item(i);
        if (item->data().toMap()["id"] == id) return item;
    }

    return nullptr;
}
