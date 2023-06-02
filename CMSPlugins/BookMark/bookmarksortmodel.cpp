/*****************************************************************************
 * bookmarksortmodel.cpp
 *
 * Created: 2022/4/24 2022 by blueghost
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
#include "bookmarksortmodel.h"

BookMarkSortModel::BookMarkSortModel(QObject* parent)
    : QSortFilterProxyModel{ parent } {}

bool
BookMarkSortModel::lessThan(const QModelIndex& source_left,
                            const QModelIndex& source_right) const {
    QString lPath = source_left.data(Qt::UserRole + 2).toString();
    QString rPath = source_right.data(Qt::UserRole + 2).toString();

    qDebug() << lPath << rPath;

    return QString::compare(lPath, rPath) < 0;
}
