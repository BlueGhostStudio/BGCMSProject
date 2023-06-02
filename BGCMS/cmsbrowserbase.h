/*****************************************************************************
 * CMSBrowserBase.h
 *
 * Created: 2022/2/7 2022 by blueghost
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
#ifndef CMSBROWSERBASE_H
#define CMSBROWSERBASE_H

#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QTableView>

#include "callgraph.h"

#define __MIME_COPY__ "cms/copy-nodes"
#define __MIME_CUT__ "cms/cut-nodes"
#define __MIME_PLUGIN__ "cms/plugin"

class CMSBrowserBase : public QTableView {
    Q_OBJECT
public:
    CMSBrowserBase(QWidget* parent = nullptr) : QTableView(parent) {}

    virtual void setFilter(
        std::function<bool(QAbstractItemModel*, int)> callback) = 0;
    virtual void load(const QVariant& pNode, CallGraph* pcg = nullptr,
                      const QString& to = QString(),
                      const QString& err = QString()) = 0;
    virtual QVariant listNode() = 0;
    virtual QVariantMap currentNode() = 0;

signals:
    void atRoot(bool r);
    void pasteContent();
    void selectedNodeChanged(const QItemSelection& selected,
                             const QItemSelection& deselected);
    void nodeItemDoubleClicked(const QVariantMap& node);
    void logMessage(const QString& message, int timeOut = 2500);
    void pathChanged(const QString& path);

    void working(bool w);
};

#endif  // CMSBROWSERBASE_H
