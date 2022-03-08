/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/3/4 2022 by blueghost
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
#ifndef CMSRESOURCEEDITOR_H
#define CMSRESOURCEEDITOR_H

#include "../cmsplugininterface.h"
#include "CMSResourceEditor_global.h"

class CMSRESOURCEEDITOR_EXPORT CMSResourceEditor : public QObject,
                                                   public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "CMSResourceEditor.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    CMSResourceEditor(QObject* parent = nullptr);

    pluginUI initial(CMSApi* api, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

signals:
    void working(bool w);

private slots:
    void setActionEnabled();

    /*private:
        QVariantMap currentNode() const;*/

private:
    QAction* m_edit;
};

#endif  // CMSRESOURCEEDITOR_H
