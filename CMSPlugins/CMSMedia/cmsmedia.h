/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/2/26 2022 by blueghost
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
#ifndef CMSMEDIA_H
#define CMSMEDIA_H

#include <QSettings>

#include "../cmsplugininterface.h"
#include "CMSMedia_global.h"
#include "cmsmediaapi.h"
#include "imageviewer.h"

class CMSMEDIA_EXPORT CMSMedia : public QObject, public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "CMSMedia.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    CMSMedia(QObject* parent = nullptr);

    pluginUI initial(CMSApi* api, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

private slots:
    void openMedia(const QVariantMap& node);
    void postMedia();
    void setActionEnabled();

private:
    CMSMediaApi m_mediaApi;
    //    ImageViewer* m_imageViewer;
    QSettings m_settings;
    QAction* m_actionCopyImageUrl;
};

#endif  // CMSMEDIA_H
