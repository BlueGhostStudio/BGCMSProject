/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/4/22 2022 by blueghost
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
#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QListView>
#include <QStandardItemModel>

#include "../cmsplugininterface.h"
#include "BookMark_global.h"
#include "bookmarksortmodel.h"

class BOOKMARK_EXPORT BookMark : public QObject, public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "BookMark.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    BookMark(QObject* parent = nullptr);

    pluginUI initial(CMSApi* api, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

private:
    int findItem(const QVariant& node);
    void addBookMarkItem(const QVariant& node, CallGraph* pcg,
                         const QString& to);
    void loadBookMarks();
    void updateBookMark();

private:
    QListView* m_bookmarkListView;
    QStandardItemModel m_bookmarkModel;
    BookMarkSortModel m_bookmarkSortModel;
    QString m_bookmarkNode;
};

#endif  // BOOKMARK_H
