/*****************************************************************************
 *  todoitemdelegate.h
 *
 *  Created: 2022/4/14 2022 by blueghost
 *
 *  Copyright 2022 blueghost. All rights reserved.
 *
 *  This file may be distributed under the terms of GNU Public License version
 *  3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 *  license should have been included with this file, or the project in which
 *  this file belongs to. You may also find the details of GPL v3 at:
 *  http://www.gnu.org/licenses/gpl-3.0.txt
 *
 *  If you have any questions regarding the use of this file, feel free to
 *  contact the author of this file, or the owner of the project in which
 *  this file belongs to.
 * ****************************************************************************/
#ifndef TODOITEMDELEGATE_H
#define TODOITEMDELEGATE_H

#include <QStack>
#include <QStyledItemDelegate>

class TodoItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit TodoItemDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;
    void setEditorData(QWidget* editor,
                       const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

signals:
    void removeTodo(const QModelIndex& index);
    void updateTodo(const QModelIndex& index, const QString& field,
                    const QVariant& data);

protected slots:
    void emitCloseEditor();
    /*protected slots:
        void emitCommitData();*/
};

#endif  // TODOITEMDELEGATE_H
