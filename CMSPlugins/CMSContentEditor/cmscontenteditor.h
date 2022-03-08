/*****************************************************************************
 * %{Cpp:License:FileName}
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
#ifndef CMSCONTENTEDITOR_H
#define CMSCONTENTEDITOR_H

#include "../cmsplugininterface.h"
#include "CMSContentEditor_global.h"

class CMSCONTENTEDITOR_EXPORT CMSContentEditor : public QObject,
                                                 public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "CMSContentEditor.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    CMSContentEditor(QObject* parent = nullptr);

    pluginUI initial(CMSApi* api, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

private:
    void newSourceNode(const QString& contentType,
                       const QString& content = QString());
    void updateNewNodeMenu();

private slots:
    void openNode(const QVariantMap& node);
    // void pasteSource();

signals:
    void logMessage(const QString& message, int timeOut = 2500);

private:
    QMenu* m_sourceSubNewNodeMenu;
    QSettings m_settings;
};

#endif  // CMSCONTENTEDITOR_H
