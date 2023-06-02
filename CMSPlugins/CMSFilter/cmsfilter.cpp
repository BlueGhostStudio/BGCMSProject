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

    /*m_filterForm = new FilterForm;

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
    _pluginUI.docker = m_filterForm;*/

    initialTreeView();
    updateContentTypeList();

    browser->setFilter([=](QAbstractItemModel* source, int row) -> bool {
        QVariantMap node = source->index(row, 0).data(Qt::UserRole + 1).toMap();

        QString type = node["type"].toString();
        QString contentType = node["contentType"].toString();
        int id = node["id"].toInt();
        int hide = node["hide"].toInt();
        int _private = node["private"].toInt();

        bool contentTypeChecked = false;
        if (type == 'D' || contentType.isEmpty())
            contentTypeChecked = true;
        else {
            if (type == 'R' && contentType == "ref")
                contentTypeChecked = true;
            else if (m_allContentFilter.checkState() == Qt::Checked)
                contentTypeChecked = true;
            else
                for (int i = 0; i < m_contentTypeFilter.rowCount(); i++) {
                    QStandardItem* item = m_contentTypeFilter.child(i);
                    if (item->text() == contentType) {
                        contentTypeChecked = item->checkState() == Qt::Checked;
                        break;
                    }
                }
        }

        bool dotdotChecked =
            type == "D" && id <= 0
                ? (m_dotdotDirFilter.checkState() == Qt::Checked)
                : true;

        bool hideChecked =
            hide ? m_hidenFilter.checkState() == Qt::Checked : true;

        bool privateChecked =
            _private ? m_privateFilter.checkState() == Qt::Checked : true;

        return contentTypeChecked && dotdotChecked && hideChecked &&
               privateChecked;
        /*QString type = node["type"].toString();
        QString contentType = node["contentType"].toString();
        int id = node["id"].toInt();
        int hide = node["hide"].toInt();
        int _private = node["private"].toInt();

        bool contentTypeChecked = false;
        if (type == 'D' || contentType.isEmpty())
            contentTypeChecked = true;
        else {
            if (ui_chbAllContentTypes->isChecked())
                contentTypeChecked = true;
            else
                for (int i = 0; i < ui_lwContentTypes->count(); i++) {
                    QListWidgetItem* item = ui_lwContentTypes->item(i);
                    if (item->text() == contentType) {
                        contentTypeChecked = (item->checkState() ==
        Qt::Checked); break;
                    }
                }
        }

        bool dotdotChecked =
            type == "D" && id <= 0 ? ui_chbDotDot->isChecked() : true;

        bool hideChecked = hide ? ui_chbHide->isChecked() : true;
        bool privateChecked = _private ? ui_chbPrivate->isChecked() : true;

        return contentTypeChecked && dotdotChecked && hideChecked &&
        privateChecked;*/
    });

    QString settingsFile = QSettings().fileName();
    m_settingsWatch.addPath(settingsFile);
    QObject::connect(&m_settingsWatch, &QFileSystemWatcher::fileChanged, this,
                     [=]() {
                         updateContentTypeList();
                         emit activateFilter();
                         m_settingsWatch.addPath(settingsFile);
                     });
    QObject::connect(
        &m_filterModel, &QStandardItemModel::itemChanged, this,
        [=](QStandardItem* item) {
            if (!item->parent() && item->row() == 0 &&
                !m_filterModel.property("skipFilter").toBool()) {
                m_filterModel.setProperty("skipFilter", true);
                for (int i = 0; i < item->rowCount(); i++)
                    item->child(i)->setCheckState(item->checkState());
                m_filterModel.setProperty("skipFilter", QVariant());
                emit activateFilter();
            } else if (!m_filterModel.property("skipFilter").toBool()) {
                emit activateFilter();

                QStandardItem* parentItem = item->parent();
                if (parentItem) {
                    m_filterModel.setProperty("skipFilter", true);

                    int uncheckedCount = 0;
                    int checkedCount = 0;
                    for (int i = 0; i < parentItem->rowCount(); i++) {
                        if (parentItem->child(i)->checkState() == Qt::Checked)
                            checkedCount++;
                        else
                            uncheckedCount++;

                        if (checkedCount > 0 && uncheckedCount > 0) break;
                    }

                    if (uncheckedCount == 0)
                        parentItem->setCheckState(Qt::Checked);
                    else if (checkedCount == 0)
                        parentItem->setCheckState(Qt::Unchecked);
                    else
                        parentItem->setCheckState(Qt::PartiallyChecked);

                    m_filterModel.setProperty("skipFilter", false);
                }
            }
        });
    QObject::connect(this, &CMSFilter::activateFilter, this, [=]() {
        QSortFilterProxyModel* filterModel =
            qobject_cast<QSortFilterProxyModel*>(browser->model());
        if (filterModel) filterModel->invalidate();
    });

    pluginUI _pluginUI;
    _pluginUI.docker = m_filterTreeView;

    return _pluginUI;
}

void
CMSFilter::initialTreeView() {
    m_filterTreeView = new QTreeView;
    m_filterTreeView->setObjectName("CMSFilter");
    m_filterTreeView->setWindowTitle(tr("CMS Filter"));

    m_filterTreeView->setHeaderHidden(true);

    m_filterTreeView->setModel(&m_filterModel);

    m_contentTypeFilter.setText(tr("Content Types"));
    m_contentTypeFilter.setCheckable(true);
    m_contentTypeFilter.setCheckState(Qt::Checked);
    m_filterModel.appendRow(&m_contentTypeFilter);

    m_allContentFilter.setText(tr("All Content"));
    m_allContentFilter.setCheckable(true);
    m_allContentFilter.setCheckState(Qt::Unchecked);
    m_filterModel.appendRow(&m_allContentFilter);

    m_hidenFilter.setText(tr("Hidden"));
    m_hidenFilter.setCheckable(true);
    m_hidenFilter.setCheckState(Qt::Checked);
    m_filterModel.appendRow(&m_hidenFilter);

    m_privateFilter.setText(tr("Private Contents"));
    m_privateFilter.setCheckable(true);
    m_privateFilter.setCheckState(Qt::Checked);
    m_filterModel.appendRow(&m_privateFilter);

    m_dotdotDirFilter.setText(tr("\"..\"Node"));
    m_dotdotDirFilter.setCheckable(true);
    m_dotdotDirFilter.setCheckState(Qt::Checked);
    m_filterModel.appendRow(&m_dotdotDirFilter);
}

void
CMSFilter::updateContentTypeList() {
    m_contentTypeFilter.removeRows(0, m_contentTypeFilter.rowCount());

    /*QStandardItem* test1 = new QStandardItem("test1");
    test1->setCheckable(true);
    test1->setCheckState(Qt::Checked);
    m_contentTypeFilter.appendRow(test1);*/
    QSettings settings;

    settings.beginGroup("contentType");
    QStringList types;
    foreach (const QString& key, settings.childKeys()) {
        types.append(settings.value(key).toStringList());
    }
    settings.endGroup();

    foreach (const QVariant& type, types) {
        QStandardItem* item = new QStandardItem(type.toString());
        item->setCheckable(true);
        item->setCheckState(Qt::Checked);
        m_contentTypeFilter.appendRow(item);
    }
}
