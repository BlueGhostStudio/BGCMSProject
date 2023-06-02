/*****************************************************************************
 * cmssortfiltermodel.cpp
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
#include "cmssortfiltermodel.h"

#include <QDebug>

CMSSortFilterModel::CMSSortFilterModel(QObject* parent)
    : QSortFilterProxyModel{ parent } {}

void
CMSSortFilterModel::setFilter(
    std::function<bool(QAbstractItemModel*, int)> callback) {
    m_filterCallback = callback;
}

bool
CMSSortFilterModel::lessThan(const QModelIndex& source_left,
                             const QModelIndex& source_right) const {
    QVariantMap leftNode =
        sourceModel()->data(source_left, Qt::UserRole + 1).toMap();
    QVariantMap rightNode =
        sourceModel()->data(source_right, Qt::UserRole + 1).toMap();

    QString lType = leftNode["type"].toString();
    QString rType = rightNode["type"].toString();
    bool lDir = lType == 'D' ||
                (lType == 'R' && leftNode["contentType"].toString() == "ref" &&
                 leftNode["extData"].toString() == 'D');
    bool rDir = rType == 'D' ||
                (rType == 'R' && rightNode["contentType"].toString() == "ref" &&
                 rightNode["extData"].toString() == 'D');

    int lSeq = leftNode["seq"].toInt();
    int rSeq = rightNode["seq"].toInt();

    int lSeqType = lSeq == -2 ? 0 : (lSeq >= 0 ? 1 : 2);
    int rSeqType = rSeq == -2 ? 0 : (rSeq >= 0 ? 1 : 2);

    if (lDir != rDir)
        return lDir;
    else if (lSeqType != rSeqType)
        return lSeqType < rSeqType;
    else
        return lSeq < rSeq;
}

bool
CMSSortFilterModel::filterAcceptsRow(int source_row,
                                     const QModelIndex& source_parent) const {
    if (m_filterCallback == nullptr)
        return true;
    else
        return m_filterCallback(sourceModel(), source_row);
}
