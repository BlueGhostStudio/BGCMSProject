/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/3/19 2022 by blueghost
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
#include "cmspathhistory.h"

CMSPathHistory::CMSPathHistory(QObject* parent) : QObject(parent) {}

pluginUI
CMSPathHistory::initial(CMSApi* api, CMSBrowserBase* browser,
                        const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);
    pluginUI _pluginUI;

    m_historyList = new QListWidget;
    m_historyList->setWindowTitle(tr("History"));
    m_historyList->setObjectName("CMSPathHistory");
    m_historyList->setProperty("dockWidgetArea", Qt::LeftDockWidgetArea);

    QPalette palette;
    palette.setBrush(QPalette::Base, QColor(0, 0, 0, 0));
    m_historyList->setPalette(palette);
    m_historyList->setAutoFillBackground(true);
    m_historyList->setFrameShape(QFrame::NoFrame);

    QObject::connect(m_cmsBrowser, SIGNAL(pathChanged(QString)), this,
                     SLOT(addPath(QString)));
    QObject::connect(m_historyList, &QListWidget::currentItemChanged, this,
                     [=](QListWidgetItem* current, QListWidgetItem*) {
                         m_cmsBrowser->load(current->text());
                     });

    _pluginUI.docker = m_historyList;

    return _pluginUI;
}

void
CMSPathHistory::addPath(const QString& path) {
    QList<QListWidgetItem*> exist =
        m_historyList->findItems(path, Qt::MatchExactly);
    if (exist.count() > 0)
        exist[0]->setSelected(true);
    else {
        QListWidgetItem* newPath = new QListWidgetItem(path, m_historyList);
        newPath->setSelected(true);
    }
}
