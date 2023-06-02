/*****************************************************************************
 * todoeditform.h
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
#ifndef TODOEDITFORM_H
#define TODOEDITFORM_H

#include <QMenu>
#include <QModelIndex>

#include "ui_todoeditform.h"

class TodoEditForm : public QWidget, private Ui::TodoEditForm {
    Q_OBJECT

public:
    explicit TodoEditForm(const QModelIndex& index, QWidget* parent = nullptr);

    void load(const QVariantMap& todo);
    bool loaded() const;
    QVariant todoData();
    QPair<QString, QVariant> changedData();

signals:
    void dataChanged();
    void removeTodo(const QModelIndex& index);
    void updateTodo(const QModelIndex& index, const QString& field,
                    const QVariant& data);
    void closeEditor();

private:
    QPair<QString, QVariant> m_changedData;
    QVariantMap m_todoData;
    bool m_loaded = false;
    QModelIndex m_index;
    QMenu* m_stateMenu;
};

#endif  // TODOEDITFORM_H
