/*****************************************************************************
 * todosortmodel.cpp
 *
 * Created: 2022/4/19 2022 by blueghost
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
#include "todosortmodel.h"

TodoSortModel::TodoSortModel(QObject* parent)
    : QSortFilterProxyModel{ parent } {}

bool
TodoSortModel::lessThan(const QModelIndex& source_left,
                        const QModelIndex& source_right) const {
    QVariantMap leftTodo = source_left.data(Qt::UserRole + 1).toMap();
    QVariantMap rightTodo = source_right.data(Qt::UserRole + 1).toMap();

    // int leftID = leftTodo["id"].toInt();
    // int rightID = rightTodo["id"].toInt();
    int leftDate = leftTodo["date"].toInt();
    int rightDate = rightTodo["date"].toInt();
    int leftState = leftTodo["state"].toInt();
    int rightState = rightTodo["state"].toInt();
    int leftPRI = leftTodo["PRI"].toInt();
    int rightPRI = rightTodo["PRI"].toInt();

    if (leftState != rightState)
        return leftState < rightState;
    else if (leftPRI != rightPRI)
        return leftPRI < rightPRI;
    else
        return leftDate > rightDate;
}
