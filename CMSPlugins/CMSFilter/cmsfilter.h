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
#ifndef CMSFILTER_H
#define CMSFILTER_H

#include <QFileSystemWatcher>

#include "../cmsplugininterface.h"
#include "CMSFilter_global.h"
//#include "filterform.h"
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

class CMSFILTER_EXPORT CMSFilter : public QObject, public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "CMSFilter.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    CMSFilter(QObject* parent = nullptr);

    pluginUI initial(CMSApi*, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

signals:
    void activateFilter();

private:
    void initialTreeView();
    void updateContentTypeList();

private:
    // FilterForm* m_filterForm;
    QFileSystemWatcher m_settingsWatch;
    QTreeView* m_filterTreeView;
    QStandardItemModel m_filterModel;

    QStandardItem m_contentTypeFilter;
    QStandardItem m_allContentFilter;
    QStandardItem m_hidenFilter;
    QStandardItem m_dotdotDirFilter;
    QStandardItem m_privateFilter;
};

#endif  // CMSFILTER_H
