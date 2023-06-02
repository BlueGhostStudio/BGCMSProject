/*****************************************************************************
 * todolistview.h
 *
 * Created: 2022/4/15 2022 by blueghost
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
#ifndef TODOLISTVIEW_H
#define TODOLISTVIEW_H

#include <QStandardItemModel>

#include "todoapi.h"
#include "todosortmodel.h"
#include "ui_todolistview.h"

class TodoListView : public QWidget, private Ui::TodoListView {
    Q_OBJECT

public:
    explicit TodoListView(TodoApi* api, QWidget* parent = nullptr);

    void load(const QVariant& node);

private slots:
    void addTodo();
    void removeAllCompleted();

private:
    void addTodoItem(const QVariantMap& todo);
    void formatViewItem(QStandardItem* item,
                        const QVariantMap& mTodoData) const;

    QStandardItem* findItem(int id);

protected:
    QStandardItemModel m_todoModel;
    TodoSortModel m_sortModel;
    TodoApi* m_api;
    QString m_lID;
};

#endif  // TODOLISTVIEW_H
