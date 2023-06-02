/*****************************************************************************
 * cmswatch.h
 *
 * Created: 2022/2/8 2022 by blueghost
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
#ifndef CMSWATCH_H
#define CMSWATCH_H

#include <QTextBrowser>

#include "../cmsplugininterface.h"
#include "CMSWatch_global.h"

class CMSWATCH_EXPORT CMSWatch : public QObject, public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "CMSWatch.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    CMSWatch(QObject* parent = nullptr);

    pluginUI initial(CMSApi* api, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

private slots:
    void browserMessage(const QString& message);

private:
    QString rpcProgress(int count);
    QString rpcProgress();

private:
    QVariantMap m_callHistory;
    QTextBrowser* m_logBrowser;

    int m_callCount = 0;
    int m_returnCount = 0;
    int m_signalCount = 0;

    int m_pingpong = 0;
    int m_wathcPongCount = 0;
    bool m_requestReconnected = false;
};

#endif  // CMSWATCH_H
