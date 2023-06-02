/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/4/14 2022 by blueghost
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
#ifndef TODO_H
#define TODO_H

#include <QListView>

#include "../cmsplugininterface.h"
#include "Todo_global.h"
#include "todoapi.h"
#include "todolistview.h"

class TODO_EXPORT Todo : public QObject, public CMSPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.bgstudio.BGCMS.CMSPluginInterface" FILE
                          "Todo.json")
    Q_INTERFACES(CMSPluginInterface)

public:
    Todo(QObject* parent = nullptr);

    pluginUI initial(CMSApi* api, CMSBrowserBase* browser,
                     const pluginUI& ui) override;

private slots:
    void dirNodeTodo();
    void openTodo(const QVariantMap& node);

private:
    TodoListView* m_todoListView;
    TodoApi m_api;
};

#endif  // TODO_H
