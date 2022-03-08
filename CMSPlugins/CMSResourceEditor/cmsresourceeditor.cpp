/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/3/4 2022 by blueghost
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
#include "cmsresourceeditor.h"

#include <QApplication>
#include <QInputDialog>

CMSResourceEditor::CMSResourceEditor(QObject* parent) : QObject(parent) {}

pluginUI
CMSResourceEditor::initial(CMSApi* api, CMSBrowserBase* browser,
                           const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    m_edit = new QAction(QIcon(":/object.png"), tr("Edit Resource"));
    m_edit->setEnabled(false);
    ui.pluginsMenu->addAction(m_edit);
    ui.pluginsToolbar->addAction(m_edit);

    QObject::connect(m_cmsBrowser,
                     SIGNAL(selectedNodeChanged(const QItemSelection&,
                                                const QItemSelection&)),
                     this, SLOT(setActionEnabled()));
    QObject::connect(m_edit, &QAction::triggered, this, [=]() {
        bool ok = false;
        QVariantMap node = m_cmsBrowser->currentNode();
        CallGraph::start("getNodeContent", this)
            ->nodes("getNodeContent",
                    [=](QPointer<CallGraph> cg, const QVariant&) {
                        qDebug() << node;
                        m_api->node(node["id"], cg, "openEditor", "error");
                    })
            ->nodes("openEditor",
                    [=](QPointer<CallGraph> cg, const QVariant& data) {
                        bool ok = false;
                        QString content = QInputDialog::getMultiLineText(
                            qApp->activeWindow(), tr("Resource"), tr("Content"),
                            data.toMap()["content"].toString(), &ok);
                        if (ok) cg->to("saveChanged", content);
                    })
            ->nodes("saveChanged",
                    [=](QPointer<CallGraph> cg, const QVariant& data) {
                        m_api->updateNode(node["id"],
                                          QVariantMap({ { "content", data } }),
                                          cg, "end", "error");
                    })
            ->nodes("end", [=](QPointer<CallGraph> cg,
                               const QVariant&) { cg->toFinal(); })
            ->nodes("error",
                    [=](QPointer<CallGraph> cg, const QVariant& data) {
                        qDebug() << "error" << data;
                    })
            ->exec();
    });

    return pluginUI();
}

void
CMSResourceEditor::setActionEnabled() {
    if (m_cmsBrowser->selectionModel()->hasSelection()) {
        QVariantMap node = m_cmsBrowser->currentNode();
        m_edit->setEnabled(node["type"].toString() == "R");
    } else
        m_edit->setEnabled(false);
}

/*QVariantMap
CMSResourceEditor::currentNode() const {
    int row = m_cmsBrowser->currentIndex().row();
    return m_cmsBrowser->model()->index(row, 0).data(Qt::UserRole + 1).toMap();
}*/
