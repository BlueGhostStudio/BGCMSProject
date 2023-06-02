/*****************************************************************************
 * todoitemdelegate.cpp
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
#include "todoitemdelegate.h"

#include <QLabel>

#include "todoeditform.h"

TodoItemDelegate::TodoItemDelegate(QObject* parent)
    : QStyledItemDelegate{ parent } {}

QWidget*
TodoItemDelegate::createEditor(QWidget* parent,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const {
    TodoEditForm* todoEdit = new TodoEditForm(index, parent);

    /*QObject::connect(todoEdit, &TodoEditForm::dataChanged, this,
                     &TodoItemDelegate::emitCommitData);*/
    QObject::connect(todoEdit, &TodoEditForm::removeTodo, this,
                     &TodoItemDelegate::removeTodo);
    QObject::connect(todoEdit, &TodoEditForm::updateTodo, this,
                     &TodoItemDelegate::updateTodo);
    QObject::connect(todoEdit, &TodoEditForm::closeEditor, this,
                     &TodoItemDelegate::emitCloseEditor);

    return todoEdit;
}

void
TodoItemDelegate::setEditorData(QWidget* editor,
                                const QModelIndex& index) const {
    QVariantMap todo = index.data(Qt::UserRole + 1).toMap();
    TodoEditForm* todoEditForm = qobject_cast<TodoEditForm*>(editor);
    if (!todoEditForm->loaded()) todoEditForm->load(todo);
}

void
TodoItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                               const QModelIndex& index) const {
    /*TodoEditForm* todoEditForm = qobject_cast<TodoEditForm*>(editor);

    QVariant todoData = todoEditForm->todoData();

    QPair<QString, QVariant> changedData = todoEditForm->changedData();
    QVariant varChangedData;
    varChangedData.setValue(changedData);

    if (changedData.first.length() > 0)
        model->setData(index, varChangedData, Qt::UserRole + 2);*/
}

QSize
TodoItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                           const QModelIndex& index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    if (size.height() < 100) size.setHeight(100);

    return size;
}

void
TodoItemDelegate::emitCloseEditor() {
    emit closeEditor(qobject_cast<QWidget*>(sender()));
}

/*void
TodoItemDelegate::emitCommitData() {
    emit commitData(qobject_cast<QWidget*>(sender()));
}*/
