/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/3/7 2022 by blueghost
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
#include "cmsnodeinfo.h"

CMSNodeInfo::CMSNodeInfo(QObject* parent) : QObject(parent) {}

pluginUI
CMSNodeInfo::initial(CMSApi* api, CMSBrowserBase* browser, const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);
    pluginUI _pluginUI;
    m_infoForm = new InfoForm();
    m_infoForm->setObjectName("CMSNodeInfo");
    m_infoForm->setWindowTitle("Node Info");
    _pluginUI.docker = m_infoForm;

    m_infoForm->setEnabled(false);
    QObject::connect(m_cmsBrowser,
                     SIGNAL(selectedNodeChanged(const QItemSelection&,
                                                const QItemSelection&)),
                     this, SLOT(setFormEnabled()));

    QObject::connect(m_api, &CMSApi::nodeUpdated, this,
                     [=](int id, const QVariantMap& node) {
                         if (!m_node.isEmpty() && m_node["id"] == id) {
                             m_node = node;
                             m_infoForm->loadInfo(m_node);
                         }
                     });

    QObject::connect(m_infoForm, &InfoForm::updateSummary, this,
                     &CMSNodeInfo::updateSummary);
    QObject::connect(m_infoForm, &InfoForm::updateExtData, this,
                     &CMSNodeInfo::updateExtData);

    return _pluginUI;
}

void
CMSNodeInfo::setFormEnabled() {
    /*int nID = currentNode();
    bool hasSelection = m_cmsBrowser->selectionModel()->hasSelection();
    m_infoForm->setEnabled(hasSelection && nID > 0);*/

    bool hasSelection = m_cmsBrowser->selectionModel()->hasSelection();

    if (hasSelection) {
        QVariantMap node = m_cmsBrowser->currentNode();
        int nID = node["id"].toInt();
        if (nID > 0) {
            CallGraph::start("getNodeInfo", this)
                ->nodes("getNodeInfo",
                        [=](QPointer<CallGraph> cg, const QVariant&) {
                            m_api->nodeInfo(nID, cg, "updateInfo", "error");
                        })
                ->nodes(
                    "updateInfo",
                    [=](QPointer<CallGraph> cg, const QVariant& data) {
                        m_node = data.toMap();
                        m_infoForm->loadInfo(m_node);
                        cg->toFinal();
                    },
                    "error",
                    [=](QPointer<CallGraph> cg, const QVariant& data) {
                        qDebug() << data;
                        cg->toFinal();
                    })
                ->exec();
            m_infoForm->setEnabled(true);
        } else {
            m_infoForm->setEnabled(false);
            m_node.clear();
        }
    } else {
        m_infoForm->setEnabled(false);
        m_node.clear();
    }
}

void
CMSNodeInfo::updateSummary(const QString& summary) {
    CallGraph::start("updateSummary", this)
        ->nodes("updateSummary",
                [=](QPointer<CallGraph> cg, const QVariant&) {
                    m_api->updateNode(m_node["id"], { { "summary", summary } },
                                      cg, "end", "error");
                })
        ->nodes(
            "end",
            [=](QPointer<CallGraph> cg, const QVariant&) { cg->toFinal(); },
            "error",
            [=](QPointer<CallGraph> cg, const QVariant& data) {
                qDebug() << data;
                cg->toFinal();
            })
        ->exec();
}

void
CMSNodeInfo::updateExtData(const QString& extData) {
    CallGraph::start("updateExtData", this)
        ->nodes("updateExtData",
                [=](QPointer<CallGraph> cg, const QVariant&) {
                    m_api->updateNode(m_node["id"], { { "extData", extData } },
                                      cg, "end", "error");
                })
        ->nodes(
            "end",
            [=](QPointer<CallGraph> cg, const QVariant&) { cg->toFinal(); },
            "error",
            [=](QPointer<CallGraph> cg, const QVariant& data) {
                qDebug() << data;
                cg->toFinal();
            })
        ->exec();
}

/*int
CMSNodeInfo::currentNode() const {
    int row = m_cmsBrowser->currentIndex().row();
    return m_cmsBrowser->model()
        ->index(row, 0)
        .data(Qt::UserRole + 1)
        .toMap()["id"]
        .toInt();
}*/
