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
#ifndef CMSNODEINFO_H
#define CMSNODEINFO_H

#include "../cmsplugininterface.h"
#include "CMSNodeInfo_global.h"
#include "infoform.h"

class CMSNODEINFO_EXPORT CMSNodeInfo : public QObject,
                                       public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "CMSNodeInfo.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    CMSNodeInfo(QObject* parent = nullptr);

    pluginUI initial(CMSApi*, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

private slots:
    void updateNodeInfo();
    void updateSummary(const QString& summary);
    void updateExtData(const QString& extData);

    /*private:
        int currentNode() const;*/

private:
    InfoForm* m_infoForm;
    QVariantMap m_node;
};

#endif  // CMSNODEINFO_H
