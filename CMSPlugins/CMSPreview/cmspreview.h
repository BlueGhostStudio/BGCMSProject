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
#ifndef CMSPREVIEW_H
#define CMSPREVIEW_H

#include "../cmsplugininterface.h"
#include "CMSPreview_global.h"
#include "previewwindow.h"

class CMSPREVIEW_EXPORT CMSPreview : public QObject, public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "CMSPreview.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    CMSPreview(QObject* parent = nullptr);

    pluginUI initial(CMSApi* api, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

private slots:
    void changeCurrentPath(const QString& path);

private:
    PreviewWindow* m_previewWindow;
    QString m_currentPath;
};

#endif  // CMSPREVIEW_H
