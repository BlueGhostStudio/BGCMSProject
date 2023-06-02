/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/4/22 2022 by blueghost
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

#include "bookmark.h"

#include <QApplication>
#include <QJsonDocument>

BookMark::BookMark(QObject* parent) : QObject(parent) {}

pluginUI
BookMark::initial(CMSApi* api, CMSBrowserBase* browser, const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    m_bookmarkNode = "/BookMark";

    m_bookmarkListView = new QListView(qApp->activeWindow());

    QPalette palette;
    palette.setBrush(QPalette::Base, QColor(0, 0, 0, 0));
    m_bookmarkListView->setPalette(palette);
    m_bookmarkListView->setAutoFillBackground(true);
    m_bookmarkListView->setFrameShape(QFrame::NoFrame);

    m_bookmarkListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_bookmarkListView->setObjectName("bookMarks");
    m_bookmarkListView->setWindowTitle(tr("BookMarks"));

    m_bookmarkSortModel.setSourceModel(&m_bookmarkModel);
    m_bookmarkListView->setModel(&m_bookmarkSortModel);

    m_bookmarkSortModel.sort(0);

    ui.pluginsToolbar->addAction(
        QIcon::fromTheme("bookmark-new"), tr("Add bookmark"), this, [=]() {
            int row = findItem(m_cmsBrowser->listNode());

            if (row == -1) {
                CallGraph::start("addItem", this)
                    ->nodes("addItem",
                            [=](CallGraph* cg, const QVariant&) {
                                addBookMarkItem(m_cmsBrowser->listNode(), cg,
                                                "updateBookMark");
                            })
                    ->nodes("updateBookMark",
                            [=](CallGraph* cg, const QVariant&) {
                                updateBookMark();
                            })
                    ->exec();
            } else {
                m_bookmarkModel.removeRow(row);
                updateBookMark();
            }
        });

    QObject::connect(m_bookmarkListView, &QListView::clicked, this,
                     [=](const QModelIndex& index) {
                         m_cmsBrowser->load(index.data(Qt::UserRole + 1));
                     });

    /*QObject::connect(m_api, &CMSApi::isConnectedChanged, this, [=](bool state)
    { if (state) loadBookMarks();
    });*/
    QObject::connect(m_api, &CMSApi::logined, this, [=]() { loadBookMarks(); });

    QObject::connect(
        m_api, &CMSApi::nodeUpdated, this,
        [=](int id, const QVariantMap& node) {
            int row = findItem(id);
            if (row != -1) {
                QStandardItem* item = m_bookmarkModel.item(row);
                QString title = node["title"].toString();
                if (!title.isEmpty())
                    item->setText(title);
                else {
                    CallGraph::start("getPath", this)
                        ->nodes("getPath",
                                [=](CallGraph* cg, const QVariant&) {
                                    m_api->nodePath(id, cg, "setItemText");
                                })
                        ->nodes(
                            "setItemText",
                            [=](CallGraph* cg, const QVariant& data) {
                                item->setText(data.toMap()["str"].toString());
                            })
                        ->exec();
                }
            }
        });

    pluginUI _pluginUI;

    _pluginUI.docker = m_bookmarkListView;

    return _pluginUI;
}

int
BookMark::findItem(const QVariant& node) {
    int row = 0;
    for (; row < m_bookmarkModel.rowCount(); row++) {
        QStandardItem* item = m_bookmarkModel.item(row);
        if (item->data() == node) return row;
    }

    return -1;
}

void
BookMark::addBookMarkItem(const QVariant& node, CallGraph* pcg,
                          const QString& to) {
    /*CallGraph::start("getInfo", this)
        ->nodes("getInfo",
                [=](CallGraph* cg, const QVariant&) {
                    m_api->nodeInfo(node, cg, "saveNodeInfo", "end");
                })
        ->nodes("saveNodeInfo",
                [=](CallGraph* cg, const QVariant& data) {
                    cg->setProperty("nodeInfo", data);
                    cg->to("getPath");
                })
        ->nodes("getPath",
                [=](CallGraph* cg, const QVariant&) {
                    m_api->nodePath(node, cg, "savePath");
                })
        ->nodes("savePath",
                [=](CallGraph* cg, const QVariant& data) {
                    cg->setProperty("path", data.toMap()["str"]);
                    cg->to("addItem");
                })
        ->nodes("addItem",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantMap nodeInfo = cg->property("nodeInfo").toMap();
                    QString path = cg->property("path").toString();

                    QStandardItem* newItem = new QStandardItem(path);

                    QString title = nodeInfo["title"].toString();

                    if (!title.isEmpty()) newItem->setText(title);
                    newItem->setData(nodeInfo["id"]);
                    newItem->setData(path, Qt::UserRole + 2);

                    m_bookmarkModel.appendRow(newItem);

                    cg->to("end");
                })
        ->nodes("end",
                [=](CallGraph* cg, const QVariant&) {
                    if (pcg) pcg->to(to);

                    cg->toFinal();
                })
        ->exec();*/

    CallGraph::start("start", this)
        ->nodes("start",
                [=](CallGraph* cg, const QVariant&) {
                    m_api->nodeInfo(node, cg, "saveNodeInfo", "end");
                    m_api->nodePath(node, cg, "savePath");
                })
        ->nodes(
            "saveNodeInfo",
            [=](CallGraph* cg, const QVariant& data) {
                cg->setProperty("nodeInfo", data);
                cg->to("addItem");
            },
            "savePath",
            [=](CallGraph* cg, const QVariant& data) {
                cg->setProperty("path", data.toMap()["str"]);
                cg->to("addItem");
            })
        ->nodes("addItem",
                [=](CallGraph* cg, const QVariant&) {
                    QVariant nodeInfo = cg->property("nodeInfo");
                    QVariant path = cg->property("path");

                    if (nodeInfo.isValid() && path.isValid()) {
                        QString strPath = path.toString();
                        QStandardItem* newItem = new QStandardItem(strPath);

                        QVariantMap mNodeInfo = nodeInfo.toMap();
                        QString title = mNodeInfo["title"].toString();

                        if (!title.isEmpty()) newItem->setText(title);
                        newItem->setData(mNodeInfo["id"]);
                        newItem->setData(strPath, Qt::UserRole + 2);

                        m_bookmarkModel.appendRow(newItem);

                        cg->to("end");
                    }
                })
        ->nodes("end",
                [=](CallGraph* cg, const QVariant&) {
                    if (pcg) pcg->to(to);
                    cg->toFinal();
                })
        ->exec();
}

void
BookMark::loadBookMarks() {
    // m_bookmarkNode = "/BookMark";

    m_bookmarkModel.clear();

    CallGraph::start("testBookMark", this)
        ->nodes("testBookMark",
                [=](CallGraph* cg, const QVariant&) {
                    m_api->node(m_bookmarkNode, cg, "load", "newBookMark");
                })
        ->nodes(
            "load",
            [=](CallGraph* cg, const QVariant& data) {
                QJsonArray jsonArray =
                    QJsonDocument::fromJson(
                        data.toMap()["content"].toByteArray())
                        .array();

                foreach (const QJsonValue& value, jsonArray) {
                    QVariant node = value.toVariant();
                    addBookMarkItem(node, nullptr, QString());
                }
                cg->to("end");
            },
            "newBookMark",
            [=](CallGraph* cg, const QVariant&) {
                m_api->addNode("/",
                               { { "name", "BookMark" },
                                 { "type", "F" },
                                 { "contentType", "txt" },
                                 { "content", "[]" },
                                 { "hide", true },
                                 { "private", true } },
                               cg, "end", "end");
            })
        ->nodes("end", [=](CallGraph* cg, const QVariant&) { cg->toFinal(); })
        ->exec();
}

void
BookMark::updateBookMark() {
    QJsonArray jsonArray;

    for (int row = 0; row < m_bookmarkModel.rowCount(); row++) {
        jsonArray.append(
            QJsonValue::fromVariant(m_bookmarkModel.item(row)->data()));
    }

    CallGraph::start("updateBookMark", this)
        ->nodes("updateBookMark",
                [=](CallGraph* cg, const QVariant&) {
                    m_api->updateNode(
                        m_bookmarkNode,
                        { { "content", QJsonDocument(jsonArray).toJson() } },
                        cg, "end", "end");
                })
        ->nodes("end", [=](CallGraph* cg, const QVariant&) { cg->toFinal(); })
        ->exec();
}
