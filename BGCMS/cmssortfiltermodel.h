/*****************************************************************************
 * cmssortfiltermodel.h
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
#ifndef CMSSORTFILTERMODEL_H
#define CMSSORTFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <functional>

class CMSSortFilterModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit CMSSortFilterModel(QObject* parent = nullptr);

    void setFilter(std::function<bool(QAbstractItemModel*, int)> callback);

protected:
    bool lessThan(const QModelIndex& source_left,
                  const QModelIndex& source_right) const override;
    bool filterAcceptsRow(int source_row,
                          const QModelIndex& source_parent) const override;

private:
    std::function<bool(QAbstractItemModel*, int)> m_filterCallback;
};

#endif  // CMSSORTFILTERMODEL_H
