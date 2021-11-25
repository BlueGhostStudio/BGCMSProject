#include "cmsfilterproxymodel.h"

#include <QDateTime>
#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>

CMSFilterProxyModel::CMSFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent) {}

bool
CMSFilterProxyModel::filterArticle() const {
    return m_filterArticle;
}

void
CMSFilterProxyModel::setFilterArticle(bool f) {
    m_filterArticle = f;
    emit filterArticleChanged(f);
    invalidateFilter();
}

bool
CMSFilterProxyModel::filterComponent() const {
    return m_filterComponent;
}

void
CMSFilterProxyModel::setFilterComponent(bool f) {
    m_filterComponent = f;
    emit filterComponentChanged(f);
    invalidateFilter();
}

bool
CMSFilterProxyModel::filterStyle() const {
    return m_filterStyle;
}

void
CMSFilterProxyModel::setFilterStyle(bool f) {
    m_filterStyle = f;
    emit filterStyleChanged(f);
    invalidateFilter();
}

bool
CMSFilterProxyModel::filterMedia() const {
    return m_filterMedia;
}

void
CMSFilterProxyModel::setFilterMedia(bool f) {
    m_filterMedia = f;
    emit filterMediaChanged(f);
    invalidateFilter();
}

bool
CMSFilterProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex& /*source_parent*/) const {
    QStandardItemModel* _sourceModel_ =
        qobject_cast<QStandardItemModel*>(sourceModel());

    QStandardItem* item = _sourceModel_->item(source_row);
    if (item->data(Qt::UserRole + 2) != 2)
        return true;
    else {
        QString type = item->data().toMap()["type"].toString();
        if (type == "A") {
            if (m_filterArticle)
                return true;
            else
                return false;
        } else if (type == "C") {
            if (m_filterComponent)
                return true;
            else
                return false;
        } else if (type == "S") {
            if (m_filterStyle)
                return true;
            else
                return false;
        } else if (type == "M") {
            if (m_filterMedia)
                return true;
            else
                return false;
        } else
            return false;
    }
}

bool
CMSFilterProxyModel::lessThan(const QModelIndex& source_left,
                              const QModelIndex& source_right) const {
    QVariantMap lData =
        sourceModel()->data(source_left, Qt::UserRole + 1).toMap();
    int lSeq = lData["seq"].toInt();
    QVariantMap rData =
        sourceModel()->data(source_right, Qt::UserRole + 1).toMap();
    int rSeq = rData["seq"].toInt();

    int sourceType = sourceModel()->data(source_left, Qt::UserRole + 2).toInt();

    /*if (allNotSet)
        return false;
    else if (allSet) {
        if (lSeq == rSeq)
            return lData["date"].toDateTime() < rData["date"].toDateTime();
        else
            return lSeq < rSeq;
    } else
        return lSeq >= 0;*/
    if (sourceType == 0) {
        if (lSeq < 0)
            return false;
        else if (rSeq < 0)
            return true;
        else
            return lSeq < rSeq;
    } else if (sourceType == 1) {
        if (lData["id"].toInt() == -1)
            return true;
        else if (rData["id"].toInt() == -1)
            return false;
        else if (lSeq < 0)
            return false;
        else if (rSeq < 0)
            return true;
        else
            return lSeq < rSeq;
    } else if (sourceType == 2) {
        if (lSeq < 0)
            return false;
        else if (rSeq < 0)
            return true;
        else {
            if (lSeq == rSeq)
                return lData["date"].toDateTime() < rData["date"].toDateTime();
            else
                return lSeq < rSeq;
        }
    } else
        return false;
}
