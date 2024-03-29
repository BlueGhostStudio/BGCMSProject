/*****************************************************************************
 * cmsbrowser.cpp
 *
 * Created: 2022/1/18 2022 by blueghost
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
#include "cmsbrowser.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QRegularExpression>

// #include "newnodedialog.h"

CMSBrowser::CMSBrowser(QWidget* parent) : CMSBrowserBase(parent) {}

void
CMSBrowser::initial(CMSApi* api) {
    m_api = api;

    m_cmsModel.setHorizontalHeaderItem(0, new QStandardItem(tr("name")));
    m_cmsModel.setHorizontalHeaderItem(1, new QStandardItem(tr("Title")));
    m_cmsModel.setHorizontalHeaderItem(2,
                                       new QStandardItem(tr("Content Type")));
    m_cmsModel.setHorizontalHeaderItem(3, new QStandardItem(tr("Hide")));
    m_cmsModel.setHorizontalHeaderItem(4, new QStandardItem(tr("Private")));
    m_cmsModel.setHorizontalHeaderItem(5, new QStandardItem(tr("Own")));
    m_cmsModel.setHorizontalHeaderItem(6, new QStandardItem(tr("Date")));
    m_cmsModel.setHorizontalHeaderItem(7,
                                       new QStandardItem(tr("Modified date")));
    m_cmsModel.setHorizontalHeaderItem(8, new QStandardItem(tr("Seq")));

    m_cmsSortFilterModel.setSourceModel(&m_cmsModel);
    m_cmsSortFilterModel.sort(0);
    m_selModel.setModel(&m_cmsSortFilterModel);
    setModel(&m_cmsSortFilterModel);
    setSelectionModel(&m_selModel);

    QHeaderView* headerView = horizontalHeader();
    headerView->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(1, QHeaderView::Stretch);
    headerView->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(8, QHeaderView::ResizeToContents);

    QObject::connect(this, &CMSBrowser::doubleClicked, this,
                     &CMSBrowser::onNodeItemDoubleClicked);
    QObject::connect(&m_cmsModel, &QStandardItemModel::itemChanged, this,
                     &CMSBrowser::onNodeItemChanged);
    QObject::connect(&m_selModel, &QItemSelectionModel::selectionChanged, this,
                     &CMSBrowser::selectedNodeChanged);

    QObject::connect(m_api, &CMSApi::nodeCreated, this,
                     [=](const QVariantMap& newNode) {
                         if (isListNode(newNode["pid"]))
                             addNodeListItem(newNode);
                     });
    QObject::connect(m_api, &CMSApi::nodeRenamed, this,
                     [=](const QVariant& node, const QString& name) {
                         if (!m_listNode.isNull() && m_pathIDs.contains(node))
                             updatePathChanged(node);
                     });
    QObject::connect(m_api, &CMSApi::nodeRemoved, this,
                     [=](const QVariant& node) {
                         if (!m_listNode.isNull() && m_pathIDs.contains(node))
                             load("/");
                         else {
                             QModelIndex index = findNodeListItem(node);
                             if (index.isValid())
                                 m_cmsModel.removeRow(index.row());
                         }
                         /*if (m_listNode == node)
                             load("/");
                         else {
                             QModelIndex index = findNodeListItem(node);
                             if (index.isValid())
                                 m_cmsModel.removeRow(index.row());
                         }*/
                     });
    QObject::connect(m_api, &CMSApi::nodeUpdated, this,
                     [=](int id, const QVariantMap& node) {
                         QModelIndex index = findNodeListItem(id);
                         if (index.isValid())
                             updateNodeListItem(index.row(), node);
                     });
    QObject::connect(m_api, &CMSApi::nodeCopied, this,
                     [=](const QVariantMap& node) {
                         if (isListNode(node["pid"])) addNodeListItem(node);
                     });
    QObject::connect(
        m_api, &CMSApi::nodeMoved, this,
        [=](const QVariantMap& node, const QVariant& targetNode) {
            QVariant mnID = node["id"];

            qDebug() << mnID << m_listNode;
            if (mnID == m_listNode) {
                QVariantMap pNode =
                    m_cmsModel.index(0, 0).data(Qt::UserRole + 1).toMap();
                if (pNode["id"].toInt() < 0) {
                    if (targetNode.isNull())
                        pNode["id"] = targetNode;
                    else
                        pNode["id"] = -targetNode.toInt();
                    updateNodeListItem(0, pNode);
                }
            }

            if (!m_listNode.isNull() && m_pathIDs.contains(mnID))
                updatePathChanged(mnID);

            QModelIndex index = findNodeListItem(mnID);
            if (index.isValid()) m_cmsModel.removeRow(index.row());
            if (isListNode(targetNode)) addNodeListItem(node);
        });
}

bool
CMSBrowser::hasSelection() const {
    return m_selModel.hasSelection();
}

void
CMSBrowser::load(const QVariant& pNode, CallGraph* pcg, const QString& to,
                 const QString& err) {
    CallGraph::start("loadListData", this)
        ->nodes("loadListData",
                [=](CallGraph* cg, const QVariant& /* data*/) {
                    // setEnabled(false);
                    emit working(true);
                    // loadNodes(m_cmsClient, data, cg, "loaded", "loaderr");
                    m_api->loadNodes(pNode, cg, "loaded", "loaderr");
                })
        ->nodes(
            "loaded",
            [=](CallGraph* cg, const QVariant& data) {
                QVariantMap m = data.toMap();
                if (!m["id"].isNull())
                    m_listNode = qAbs(m["id"].toInt());
                else
                    m_listNode = QVariant::fromValue(nullptr);

                // m_pathIDs = m["pathIDs"].toList();
                QVariantMap path = m["path"].toMap();
                m_pathIDs = path["ids"].toList();

                emit pathChanged(path["str"].toString());

                m_cmsModel.removeRows(0, m_cmsModel.rowCount());
                QVariantList list = m["list"].toList();
                foreach (const QVariant& nodeData, list) {
                    addNodeListItem(nodeData.toMap());
                }
                // m_cmsSortFilterModel.sort(0);
                m_cmsSortFilterModel.invalidate();

                if (pcg /*.isNull()*/ && !to.isEmpty()) pcg->to(to);

                cg->to("end");
            },
            "loaderr",
            [=](CallGraph* cg, const QVariant& data) {
                if (pcg /*.isNull()*/ && !err.isEmpty()) pcg->to(err, data);

                emit logMessage(tr("ERROR: ") + data.toString());

                cg->to("end");
            })
        ->nodes("end",
                [=](CallGraph* cg, const QVariant& data) {
                    // setEnabled(true);
                    emit working(false);
                    emit atRoot(m_listNode.isNull());
                    cg->toFinal();
                })
        ->exec();
}

void
CMSBrowser::setFilter(std::function<bool(QAbstractItemModel*, int)> callback) {
    m_cmsSortFilterModel.setFilter(callback);
    m_cmsSortFilterModel.invalidate();
}

QVariant
CMSBrowser::listNode() {
    return m_listNode;
}

QVariantMap
CMSBrowser::currentNode() {
    // int row = currentIndex().row();
    int row = m_cmsSortFilterModel.mapToSource(currentIndex()).row();
    //    qDebug() << "---row---" << row;
    return m_cmsModel.index(row, 0).data(Qt::UserRole + 1).toMap();
}

void
CMSBrowser::removeSelectedNodes() {
    if (QMessageBox::question(
            qApp->activeWindow(), tr("Remove nodes"),
            tr("Do you really want to delete the selected nodes?")) ==
        QMessageBox::Yes) {
        QModelIndexList selectedNodes = m_selModel.selectedRows();

        QVariantList rmNodeIDs;
        foreach (const QModelIndex& index, selectedNodes) {
            rmNodeIDs << index.data(Qt::UserRole + 1).toMap()["id"];
        }

        while (!rmNodeIDs.isEmpty()) {
            QVariant id = rmNodeIDs.takeFirst();
            CallGraph::start("removeNode", this)
                ->nodes("removeNode",
                        [=](CallGraph* cg, const QVariant&) {
                            // setEnabled(false);
                            emit working(true);
                            m_api->removeNode(id, cg, "end", "error");
                        })
                ->nodes("error",
                        [=](CallGraph* cg, const QVariant& data) {
                            emit logMessage(tr("ERROR: ") + data.toString());
                            cg->to("end");
                        })
                ->nodes("end",
                        [=](CallGraph* cg, const QVariant&) {
                            // setEnabled(true);
                            emit working(false);
                            cg->toFinal();
                        })
                ->exec();
        }
    }
}

void
CMSBrowser::back() {
    CallGraph::start("getNode", this)
        ->nodes("getNode",
                [=](CallGraph* cg, const QVariant&) {
                    m_api->node(m_listNode, cg, "back", "error");
                })
        ->nodes(
            "back",
            [=](CallGraph* cg, const QVariant& data) {
                load(data.toMap()["pid"], cg, "end", "error");
            },
            "error",
            [=](CallGraph* cg, const QVariant& data) {
                emit logMessage(tr("ERROR: ") + data.toString());
                cg->to("end");
            })
        ->nodes("end", [=](CallGraph* cg, const QVariant&) { cg->toFinal(); })
        ->exec();
}

void
CMSBrowser::rootDirectory() {
    load("/");
}

void
CMSBrowser::copySelectedNodes() {
    /*QMimeData* mimeData = new QMimeData;

    mimeData->setData(__MIME_COPY__, selectedNodesMimeData());
    QApplication::clipboard()->setMimeData(mimeData);

    copyNodeTextMimeData(mimeData);*/
    int format = QSettings().value("copyLinkFormat", 0).toInt();

    CallGraph::start("getSelectedNodes", this)
        ->nodes("getSelectedNodes",
                [=](CallGraph* cg, const QVariant&) {
                    cg->setProperty("nodes", selectedNodesMimeData());
                    cg->to("getCurrentNode");
                })
        ->nodes("getCurrentNode",
                [=](CallGraph* cg, const QVariant&) {
                    cg->setProperty("curNode", currentNode());
                    if (format == 0 || format == 3)
                        cg->to("copyToClipboard");
                    else
                        cg->to("checkImg");
                })
        ->nodes("checkImg",
                [=](CallGraph* cg, const QVariant&) {
                    QVariantMap mData = cg->property("curNode").toMap();
                    bool isImage =
                        QSettings()
                            .value("contentType/image")
                            .toList()
                            .contains(mData["contentType"].toString());
                    cg->setProperty("isImage", isImage);
                    if (!isImage)
                        cg->to("copyToClipboard");
                    else
                        cg->to("getImgContent", mData);
                })
        ->nodes("getImgContent",
                [=](CallGraph* cg, const QVariant& node) {
                    m_api->node(node.toMap()["id"], cg, "getImgUrl",
                                "copyToClipboard");
                })
        ->nodes("getImgUrl",
                [=](CallGraph* cg, const QVariant& node) {
                    QString content = node.toMap()["content"].toString();
                    if (content.contains(QRegularExpression("^link:"))) {
                        cg->setProperty("imgUrl", content.mid(5));
                        cg->to("copyToClipboard");
                    } else if (content.contains(QRegularExpression("^res:"))) {
                        m_api
                            ->callMethod(m_api->CMSObjID("media"), "mediaURL",
                                         { content.mid(4) })
                            ->then([=](const QVariant& ret) {
                                cg->setProperty("imgUrl", ret.toList()[0]);
                                cg->to("copyToClipboard");
                            });
                    } else
                        cg->to("copyToClipboard");
                })
        ->nodes(
            "copyToClipboard",
            [=](CallGraph* cg, const QVariant&) {
                QMimeData* mimeData = new QMimeData;
                mimeData->setData(__MIME_COPY__,
                                  cg->property("nodes").toByteArray());
                QVariantMap mData = cg->property("curNode").toMap();
                bool isImage = cg->property("isImage").toBool();
                QString imgUrl = cg->property("imgUrl").toString();
                QString summary = mData["summary"].toString();
                QString title = mData["title"].toString();
                QString id = mData["id"].toString();

                QString textData;
                switch (format) {
                case 0:
                    textData = "?id=" + id;
                    break;
                case 1: {
                    QString aContent = title;
                    if (isImage)
                        aContent =
                            QString(
                                R"(<img src="%1" alt="%2" title="%3"></img>)")
                                .arg(imgUrl)
                                .arg(summary)
                                .arg(title);
                    textData = QString(R"(<a href="?id=%1" title="%2">%3</a>)")
                                   .arg(id)
                                   .arg(summary)
                                   .arg(aContent);
                } break;
                case 2: {
                    QString lContent = mData["title"].toString();
                    if (isImage)
                        lContent = QString(R"md(![%1](%2 %3))md")
                                       .arg(summary)
                                       .arg(imgUrl)
                                       .arg(title);
                    textData = QString(R"md([%1](?id=%2 %3))md")
                                   .arg(lContent)
                                   .arg(id)
                                   .arg(summary);
                } break;
                case 3:
                    textData = id;
                    break;
                default:
                    break;
                }
                mimeData->setText(textData);
                QGuiApplication::clipboard()->setMimeData(mimeData);
                cg->toFinal();
            })
        ->exec();
}

void
CMSBrowser::cutSelectedNodes() {
    QMimeData* mimeData = new QMimeData;

    mimeData->setData(__MIME_CUT__, selectedNodesMimeData());
    QApplication::clipboard()->setMimeData(mimeData);
}

void
CMSBrowser::paste() {
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();
    if (mimeData->hasFormat(__MIME_COPY__)) {
        QByteArrayList baNodeList = mimeData->data(__MIME_COPY__).split(',');
        foreach (const QByteArray& baNode, baNodeList) {
            int nNode = baNode.toInt();
            if (nNode > 0) {
                CallGraph::start("copyNode", this)
                    ->nodes("copyNode",
                            [=](CallGraph* cg, const QVariant& data) {
                                // setEnabled(false);
                                emit working(true);
                                m_api->copyNode(nNode, m_listNode, cg, "end",
                                                "error");
                            })
                    ->nodes(
                        "error",
                        [=](CallGraph* cg, const QVariant& data) {
                            emit logMessage(tr("ERROR: ") + data.toString());
                            cg->to("end");
                        })
                    ->nodes("end",
                            [=](CallGraph* cg, const QVariant&) {
                                // setEnabled(true);
                                emit working(false);
                                cg->toFinal();
                            })
                    ->exec();
            }
        }
    } else if (mimeData->hasFormat(__MIME_CUT__)) {
        QByteArrayList baNodeList = mimeData->data(__MIME_CUT__).split(',');
        foreach (const QByteArray& baNode, baNodeList) {
            int nNode = baNode.toInt();
            if (nNode > 0) {
                CallGraph::start("moveNode", this)
                    ->nodes("moveNode",
                            [=](CallGraph* cg, const QVariant& data) {
                                emit working(true);
                                m_api->moveNode(nNode, m_listNode, cg, "end",
                                                "error");
                            })
                    ->nodes(
                        "error",
                        [=](CallGraph* cg, const QVariant& data) {
                            emit logMessage(tr("ERROR: ") + data.toString());
                            cg->to("end");
                        })
                    ->nodes("end",
                            [=](CallGraph* cg, const QVariant&) {
                                // setEnabled(true);
                                emit working(false);
                                cg->toFinal();
                            })
                    ->exec();
            }
        }

        clipboard->clear();
    } else if (mimeData->hasFormat(__MIME_PLUGIN__))
        emit pasteContent();
}

void
CMSBrowser::pasteRef() {
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();
    if (mimeData->hasFormat(__MIME_COPY__)) {
        QByteArrayList baNodeList = mimeData->data(__MIME_COPY__).split(',');
        foreach (const QByteArray& baNode, baNodeList) {
            int nNode = baNode.toInt();
            if (nNode > 0) {
                CallGraph::start("copyRefNode", this)
                    ->nodes("copyRefNode",
                            [=](CallGraph* cg, const QVariant&) {
                                emit working(true);
                                m_api->copyRefNode(nNode, m_listNode, cg, "end",
                                                   "error");
                            })
                    ->nodes(
                        "error",
                        [=](CallGraph* cg, const QVariant& data) {
                            emit logMessage(tr("ERROR: ") + data.toString());
                            cg->to("end");
                        })
                    ->nodes("end",
                            [=](CallGraph* cg, const QVariant&) {
                                emit working(false);
                                cg->toFinal();
                            })
                    ->exec();
            }
        }
    }
}

QVariantMap
CMSBrowser::nodeData(const QModelIndex& index) {
    return m_cmsModel.index(index.row(), 0).data(Qt::UserRole + 1).toMap();
}

QModelIndex
CMSBrowser::findNodeListItem(const QVariant& id) {
    for (int row = 0; row <= m_cmsModel.rowCount(); ++row) {
        QModelIndex index = m_cmsModel.index(row, 0);
        if (index.data(Qt::UserRole + 1).toMap()["id"] == id) return index;
    }

    return QModelIndex();
}

void
CMSBrowser::addNodeListItem(const QVariantMap& data) {
    int newRow = m_cmsModel.rowCount();
    m_cmsModel.appendRow(QList<QStandardItem*>(7));

    updateNodeListItem(newRow, data);
}

void
CMSBrowser::updateNodeListItem(int row, const QVariantMap& data) {
    auto nodeItem = [&](int col) -> QStandardItem* {
        QStandardItem* item = m_cmsModel.item(row, col);
        if (!item) {
            item = new QStandardItem;
            m_cmsModel.setItem(row, col, item);
        }

        return item;
    };

    QString contentType = data["contentType"].toString();
    QString nType = data["type"].toString();
    bool isDir = data["type"] == "D";
    int nID = data["id"].toInt();
    QString extData = data["extData"].toString();

    QStandardItem* nameItem = nodeItem(0);
    nameItem->setData(QVariant());
    nameItem->setText(data["name"].toString());

    if (nID <= 0) nameItem->setEditable(false);

    if (isDir)
        nameItem->setIcon(QIcon(":/imgs/category.png"));
    else {
        if (nType == "R") {
            QMimeDatabase db;
            QString mimeType =
                db.mimeTypeForFile("check." + contentType).name();
            if (nType == "R" && contentType == "ref") {
                if (extData == "D")
                    nameItem->setIcon(QIcon(":/imgs/refDir.png"));
                else
                    nameItem->setIcon(QIcon(":/imgs/refFile.png"));
            } else if (mimeType.contains(QRegularExpression("^image")))
                nameItem->setIcon(QIcon(":/imgs/image.png"));
            else if (mimeType.contains(QRegularExpression("^video")))
                nameItem->setIcon(QIcon(":/imgs/video.png"));
            else if (mimeType.contains(QRegularExpression("^audio")))
                nameItem->setIcon(QIcon(":/imgs/audio.png"));
            else
                nameItem->setIcon(QIcon(":/imgs/article.png"));
        } else
            nameItem->setIcon(QIcon(":/imgs/article.png"));
    }

    QStandardItem* titleItem = nodeItem(1);
    if (nID > 0)
        titleItem->setText(data["title"].toString());
    else {
        titleItem->setText("Previous directory");
        titleItem->setEditable(false);
    }

    QStandardItem* contentTypeItem = nodeItem(2);
    contentTypeItem->setText(contentType);
    if (isDir) contentTypeItem->setEditable(false);

    QStandardItem* hideItem = nodeItem(3);
    if (nID > 0) {
        hideItem->setCheckState(data["hide"] == 0 ? Qt::Unchecked
                                                  : Qt::Checked);
        hideItem->setCheckable(true);
        // hideItem->setFlags(Qt::ItemIsUserCheckable);
        /*hideItem->setData(data["hide"] == 0 ? Qt::Unchecked : Qt::Checked,
                          Qt::CheckStateRole);*/
    } else
        hideItem->setEditable(false);

    QStandardItem* priItem = nodeItem(4);
    if (nID > 0) {
        priItem->setCheckState(data["private"] == 0 ? Qt::Unchecked
                                                    : Qt::Checked);
        priItem->setCheckable(true);
    } else
        priItem->setEditable(false);

    QStandardItem* ownItem = nodeItem(5);
    ownItem->setText(data["own"].toString());
    ownItem->setEditable(false);

    QStandardItem* dateItem = nodeItem(6);
    dateItem->setText(data["date"].toString());
    dateItem->setEditable(false);

    QStandardItem* mDateItem = nodeItem(7);
    mDateItem->setText(data["mdate"].toString());
    mDateItem->setEditable(false);

    QStandardItem* seqItem = nodeItem(8);
    if (nID > 0)
        seqItem->setText(data["seq"].toString());
    else
        seqItem->setEditable(false);

    nameItem->setData(data);
}

void
CMSBrowser::updatePathChanged(const QVariant& node) {
    CallGraph::start("nodePath", this)
        ->nodes("nodePath",
                [=](CallGraph* cg, const QVariant&) {
                    m_api->nodePath(m_listNode, cg, "pathChanged");
                })
        ->nodes("pathChanged",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantMap path = data.toMap();
                    qDebug() << path["str"];
                    emit pathChanged(path["str"].toString());
                    m_pathIDs = path["ids"].toList();
                    cg->toFinal();
                })
        ->exec();
}

void
CMSBrowser::onNodeItemDoubleClicked(const QModelIndex& index) {
    // QVariantMap _nodeData =
    // nodeData(m_cmsSortFilterModel.mapToSource(index));

    CallGraph::start("checkNodeType", this)
        ->nodes("checkNodeType",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantMap mData = data.toMap();
                    if (mData["type"] == "D") {
                        cg->to("changeDir", data);
                        emit nodeItemDoubleClicked(mData);
                    } else if (mData["type"] == "R" &&
                               mData["contentType"] == "ref")
                        cg->to("getRef", data);
                    else {
                        emit nodeItemDoubleClicked(mData);
                        cg->to("end");
                    }
                })
        ->nodes("changeDir",
                [=](CallGraph* cg, const QVariant& data) {
                    load(data.toMap()["id"], cg, "end", "error");
                })
        ->nodes("getRef",
                [=](CallGraph* cg, const QVariant& data) {
                    m_api->refNode(data.toMap()["id"], cg, "checkNodeType",
                                   "error");
                })
        ->nodes("error",
                [=](CallGraph* cg, const QVariant& data) {
                    emit logMessage(tr("ERROR: ") + data.toString());
                    cg->to("end");
                })
        ->nodes("end", [=](CallGraph* cg, const QVariant&) { cg->toFinal(); })
        ->exec(nodeData(m_cmsSortFilterModel.mapToSource(index)));

    /*if (_nodeData["type"] == "D") {
        CallGraph::start("load", this)
            ->nodes("load",
                    [=](CallGraph* cg, const QVariant& data) {
                        // qDebug() << _nodeData["id"];
                        load(_nodeData["id"], cg, "end", "loadErr");
                    })
            ->nodes("end",
                    [=](CallGraph* cg, const QVariant&) { cg->toFinal(); })
            ->nodes("loadErr",
                    [=](CallGraph* cg, const QVariant& data) {
                        emit logMessage(tr("ERROR: ") + data.toString());
                        cg->toFinal();
                    })
            ->exec();
    } else if (_nodeData["type"] == "R" && _nodeData["contentType"] == "ref") {
        CallGraph::start("getRefNode", this)
            ->nodes("getRefNode",
                    [=](QPointer<CallGraph> cg, const QVariant&) {
                        m_api->refNodeInfo(_nodeData["id"], cg, "emitSignal",
                                           "end");
                    })
            ->nodes("emitSignal",
                    [=](QPointer<CallGraph> cg, const QVariant& data) {
                        emit nodeItemDoubleClicked(data.toMap());
                        cg->to("end");
                    })
            ->nodes("end", [=](QPointer<CallGraph> cg,
                               const QVariant&) { cg->toFinal(); })
            ->exec();
    } else
        emit nodeItemDoubleClicked(_nodeData);*/
}

void
CMSBrowser::onNodeItemChanged(QStandardItem* item) {
    QModelIndex index = item->index();
    int col = index.column();
    int row = index.row();

    if (col < 5 || col == 8) {
        QVariantMap origNodeData = nodeData(index);
        int nID = origNodeData["id"].toInt();

        QVariantMap updateData;

        if (col == 0) {
            QVariant nameData = origNodeData["name"];
            if (nameData.isValid() && nameData != item->text())
                updateData["name"] = item->text();
        } else if (col == 1) {
            QVariant titleData = origNodeData["title"];
            if (titleData.isValid() && titleData != item->text())
                updateData["title"] = item->text();
        } else if (col == 2) {
            QVariant contentTypeData = origNodeData["contentType"];
            if (contentTypeData.isValid() && contentTypeData != item->text())
                updateData["contentType"] = item->text();
        } else if (col == 3) {
            QVariant hideData = origNodeData["hide"];
            int itemValue = item->checkState() == Qt::Checked ? 1 : 0;
            if (hideData.isValid() && hideData != itemValue)
                updateData["hide"] = itemValue;
        } else if (col == 4) {
            QVariant priData = origNodeData["private"];
            int itemValue = item->checkState() == Qt::Checked ? 1 : 0;
            if (priData.isValid() && priData != itemValue)
                updateData["private"] = itemValue;
        } else if (col == 8) {
            QVariant seqData = origNodeData["seq"];
            int itemValue = item->text().toInt();
            if (seqData.isValid() && seqData.toInt() != itemValue)
                updateData["seq"] = itemValue;
        }

        if (!updateData.isEmpty()) {
            CallGraph::start("updateNode", this)
                ->nodes("updateNode",
                        [=](CallGraph* cg, const QVariant&) {
                            // setEnabled(false);
                            emit working(true);
                            m_api->updateNode(nID, updateData, cg, "end",
                                              "error");
                        })
                ->nodes("error",
                        [=](CallGraph* cg, const QVariant& data) {
                            emit logMessage(tr("ERROR: ") + data.toString());
                            updateNodeListItem(row, origNodeData);
                            cg->to("end");
                        })
                ->nodes("end",
                        [=](CallGraph* cg, const QVariant&) {
                            // setEnabled(true);
                            emit working(false);
                            cg->toFinal();
                        })
                ->exec();
        }
    }
}

QByteArray
CMSBrowser::selectedNodesMimeData() {
    QModelIndexList selectedNodes = m_selModel.selectedRows();

    QByteArray nodes;
    foreach (const QModelIndex& index, selectedNodes) {
        QVariant id = index.data(Qt::UserRole + 1).toMap()["id"];
        if (id.toInt() <= 0) continue;

        if (!nodes.isEmpty()) nodes += ',';
        nodes += id.toByteArray();
    }

    return nodes;
}

void
CMSBrowser::copyNodeTextMimeData(QMimeData* mimeData) {
    CallGraph::start("getNodeInfo", this)
        ->nodes("getNodeInfo",
                [=](CallGraph* cg, const QVariant&) {
                    QVariant node = currentNode();
                    cg->setProperty("node", node);
                    cg->to("copyHref", node);
                })
        ->nodes("getUrl",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantMap mData = data.toMap();
                    mimeData->setText("this is a test");
                    QGuiApplication::clipboard()->setMimeData(mimeData);
                    cg->toFinal();
                })
        ->exec();
}

bool
CMSBrowser::isListNode(const QVariant& pID) {
    // if (m_listNode.isNull() && pID.typeId() == QMetaType::QString &&
    // pID.toString().isEmpty())
    qDebug() << pID.typeId();
    return (m_listNode.isNull() && pID.typeId() == QMetaType::QString &&
            pID.toString().isEmpty()) ||
           m_listNode.toInt() == pID.toInt();
}
