/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/3/20 2022 by blueghost
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
#include "cmspreview.h"

#include <QInputDialog>
#include <QSettings>

CMSPreview::CMSPreview(QObject* parent) : QObject(parent) {
    m_previewWindow = new PreviewWindow();
}

pluginUI
CMSPreview::initial(CMSApi* api, CMSBrowserBase* browser, const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    QAction* preview = ui.pluginsToolbar->addAction(
        QIcon(":/preview.png"), tr("Preview"), this, [=]() {
            m_previewWindow->show();

            if (m_cmsBrowser->selectionModel()->hasSelection()) {
                QVariantMap currentNode = m_cmsBrowser->currentNode();
                m_previewWindow->preview(m_currentPath + "/" +
                                         currentNode["name"].toString());
            } else
                m_previewWindow->preview(m_currentPath);
        });
    ui.pluginsMenu->addAction(preview);

    ui.settingMenu->addAction(tr("Preview Url"), this, [=]() {
        bool ok;
        QSettings settings;
        QString url = QInputDialog::getText(
            qApp->activeWindow(), tr("Preview Url"), tr("Url"),
            QLineEdit::Normal,
            settings.value("preview/url", "http://127.0.0.1").toString(), &ok);

        if (ok) settings.setValue("preview/url", url);
    });

    QObject::connect(m_cmsBrowser, SIGNAL(pathChanged(QString)), this,
                     SLOT(changeCurrentPath(QString)));

    return pluginUI();
}

void
CMSPreview::changeCurrentPath(const QString& path) {
    m_currentPath = path;
}
