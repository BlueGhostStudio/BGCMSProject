/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/2/21 2022 by blueghost
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
#include "cmsfilter.h"

#include <QAbstractItemModel>
#include <QScrollArea>
#include <QSortFilterProxyModel>

CMSFilter::CMSFilter(QObject* parent) : QObject(parent) {}

pluginUI
CMSFilter::initial(CMSApi* api, CMSBrowserBase* browser, const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    m_filterForm = new FilterForm;

    m_filterForm->updateContentTypeList();
    QString settingsFile = QSettings().fileName();
    m_settingsWatch.addPath(settingsFile);
    QObject::connect(&m_settingsWatch, &QFileSystemWatcher::fileChanged, this,
                     [=]() {
                         m_filterForm->updateContentTypeList();
                         emit m_filterForm->updateFilter();
                         m_settingsWatch.addPath(settingsFile);
                     });

    browser->setFilter([=](QAbstractItemModel* source, int row) -> bool {
        QVariantMap node = source->index(row, 0).data(Qt::UserRole + 1).toMap();

        return m_filterForm->check(node);
    });

    QObject::connect(m_filterForm, &FilterForm::updateFilter, this, [=]() {
        QSortFilterProxyModel* filterModel =
            qobject_cast<QSortFilterProxyModel*>(browser->model());
        if (filterModel) filterModel->invalidate();
    });

    pluginUI _pluginUI;

    m_filterForm->setObjectName("CMSFilter");
    m_filterForm->setWindowTitle(tr("CMS Filter"));
    _pluginUI.docker = m_filterForm;

    return _pluginUI;
}
