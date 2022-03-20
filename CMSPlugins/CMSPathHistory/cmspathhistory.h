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
#ifndef CMSPATHHISTORY_H
#define CMSPATHHISTORY_H

#include <QListWidget>

#include "../cmsplugininterface.h"
#include "CMSPathHistory_global.h"

class CMSPATHHISTORY_EXPORT CMSPathHistory : public QObject,
                                             public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "CMSPathHistory.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    CMSPathHistory(QObject* parent = nullptr);

    pluginUI initial(CMSApi* api, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

private slots:
    void addPath(const QString& path);

private:
    QListWidget* m_historyList;
};

#endif  // CMSPATHHISTORY_H
