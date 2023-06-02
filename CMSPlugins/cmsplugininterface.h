/*****************************************************************************
 * cmsplugininterface.h
 *
 * Created: 2022/1/31 2022 by blueghost
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
#ifndef CMSPLUGININTERFACE_H
#define CMSPLUGININTERFACE_H

#include <QMenu>
#include <QToolBar>
#include <QtCore>

#include "../BGCMS/cmsapi.h"
#include "../BGCMS/cmsbrowserbase.h"
// #include "../BGCMS/cmsbrowser.h"

struct pluginUI {
    QWidget* docker = nullptr;
    QMenu* settingMenu;
    QMenu* newNodeMenu;
    QMenu* pluginsMenu;
    QToolBar* pluginsToolbar;
    QMainWindow* mainWindow;
};

class CMSPluginInterface {
public:
    virtual pluginUI initial(CMSApi* api, CMSBrowserBase* browser,
                             const pluginUI& ui = pluginUI()) {
        m_api = api;
        m_cmsBrowser = browser;

        return pluginUI();
    };

protected:
    CMSApi* m_api;
    CMSBrowserBase* m_cmsBrowser;
};

Q_DECLARE_INTERFACE(CMSPluginInterface, "org.bgstudio.BGCMS.CMSPluginInterface")

#endif  // CMSPLUGININTERFACE_H
