/*****************************************************************************
 * cmsbrowser.h
 *
 * Created: 2022/1/18 2022 by BlueGhost Studio
 *
 * Copyright 2022 BlueGhost Studio. All rights reserved.
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
#ifndef CMSBROWSER_H
#define CMSBROWSER_H

#include <bgmrpcclient.h>
#include <callgraph.h>

#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QTableView>
#include <functional>

#include "cmsapi.h"
#include "cmsbrowserbase.h"
#include "cmssortfiltermodel.h"

// extern NS_BGMRPCClient::BGMRPCClient BGCMSClient;

class CMSBrowser : public CMSBrowserBase {
    Q_OBJECT
public:
    CMSBrowser(QWidget* parent = nullptr);

    void initial(CMSApi* api);

    bool hasSelection() const;
    void load(const QVariant& pNode, QPointer<CallGraph> pcg = nullptr,
              const QString& to = QString(), const QString& err = QString());
    void setFilter(
        std::function<bool(QAbstractItemModel*, int)> callback) override;

    QVariant listNode() override;
    QVariantMap currentNode() override;

    /*signals:
        void atRoot(bool r);
        void selectedNodeChanged(const QItemSelection& selected,
                                 const QItemSelection& deselected);
        void nodeItemDoubleClicked(const QVariantMap& node);
        void logMessage(const QString& message);*/

public slots:
    // void newNode(const QVariantMap& node);
    void removeSelectedNodes();
    void back();
    void rootDirectory();

    void copySelectedNodes();
    void cutSelectedNodes();
    void paste();

private slots:
    void onNodeItemDoubleClicked(const QModelIndex& index);
    void onNodeItemChanged(QStandardItem* item);

private:
    QByteArray selectedNodesMimeData();

    bool isListNode(const QVariant& pID);
    QVariantMap nodeData(const QModelIndex& index);
    QModelIndex findNodeListItem(const QVariant& id);
    void addNodeListItem(const QVariantMap& data);
    void updateNodeListItem(int row, const QVariantMap& data);

    void updatePathChanged(const QVariant& node);

protected:
    // NS_BGMRPCClient::BGMRPCClient* m_cmsClient;
    CMSApi* m_api;
    QStandardItemModel m_cmsModel;
    QItemSelectionModel m_selModel;
    CMSSortFilterModel m_cmsSortFilterModel;
    QVariant m_listNode;
    QVariantList m_pathIDs;
};

#endif  // CMSBROWSER_H
