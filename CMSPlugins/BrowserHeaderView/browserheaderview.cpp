/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/3/6 2022 by blueghost
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
#include "browserheaderview.h"

#include <QHeaderView>
#include <QSettings>

BrowserHeaderView::BrowserHeaderView(QObject* parent) : QObject(parent) {}

pluginUI
BrowserHeaderView::initial(CMSApi* api, CMSBrowserBase* browser,
                           const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    QHeaderView* headerView = m_cmsBrowser->horizontalHeader();

    auto initHeaderView = [=](int col, const QString& key, QAction* action) {
        bool show = m_settings.value(key, true).toBool();
        action->setChecked(show);
        show ? headerView->showSection(col) : headerView->hideSection(col);
    };

    QMenu* HVMenu = ui.settingMenu->addMenu(tr("Header View Columns"));
    QAction* actName = HVMenu->addAction(tr("Name"), this, [=](bool checked) {
        if (checked)
            headerView->showSection(0);
        else
            headerView->hideSection(0);

        m_settings.setValue("browser/header/name", checked);
    });
    actName->setCheckable(true);
    initHeaderView(0, "/browser/header/name", actName);

    QAction* actTitle = HVMenu->addAction(tr("Title"), this, [=](bool checked) {
        if (checked)
            headerView->showSection(1);
        else
            headerView->hideSection(1);

        m_settings.setValue("browser/header/title", checked);
    });
    actTitle->setCheckable(true);
    initHeaderView(1, "/browser/header/title", actTitle);

    QAction* actContentType =
        HVMenu->addAction(tr("Content Type"), this, [=](bool checked) {
            if (checked)
                headerView->showSection(2);
            else
                headerView->hideSection(2);

            m_settings.setValue("browser/header/contentType", checked);
        });
    actContentType->setCheckable(true);
    initHeaderView(2, "/browser/header/contentType", actContentType);

    QAction* actHide = HVMenu->addAction(tr("Hide"), this, [=](bool checked) {
        if (checked)
            headerView->showSection(3);
        else
            headerView->hideSection(3);

        m_settings.setValue("browser/header/hide", checked);
    });
    actHide->setCheckable(true);
    initHeaderView(3, "/browser/header/hide", actHide);

    QAction* actPrivate =
        HVMenu->addAction(tr("Private"), this, [=](bool checked) {
            if (checked)
                headerView->showSection(4);
            else
                headerView->hideSection(4);

            m_settings.setValue("browser/header/private", checked);
        });
    actPrivate->setCheckable(true);
    initHeaderView(4, "/browser/header/private", actPrivate);

    QAction* actOwn = HVMenu->addAction(tr("Own"), this, [=](bool checked) {
        if (checked)
            headerView->showSection(5);
        else
            headerView->hideSection(5);

        m_settings.setValue("browser/header/own", checked);
    });
    actOwn->setCheckable(true);
    initHeaderView(5, "/browser/header/own", actOwn);

    QAction* actDate = HVMenu->addAction(tr("Date"), this, [=](bool checked) {
        if (checked)
            headerView->showSection(6);
        else
            headerView->hideSection(6);

        m_settings.setValue("browser/header/date", checked);
    });
    actDate->setCheckable(true);
    initHeaderView(6, "/browser/header/date", actDate);

    QAction* actMDate =
        HVMenu->addAction(tr("Modified Date"), this, [=](bool checked) {
            if (checked)
                headerView->showSection(7);
            else
                headerView->hideSection(7);

            m_settings.setValue("browser/header/mdate", checked);
        });
    actMDate->setCheckable(true);
    initHeaderView(7, "/browser/header/mdate", actMDate);

    QAction* actSeq = HVMenu->addAction(tr("Seq"), this, [=](bool checked) {
        if (checked)
            headerView->showSection(8);
        else
            headerView->hideSection(8);

        m_settings.setValue("browser/header/seq", checked);
    });
    actSeq->setCheckable(true);
    initHeaderView(8, "/browser/header/seq", actSeq);

    return pluginUI();
}
