#ifndef CMSFILTERPROXYMODEL_H
#define CMSFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class CMSFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

    Q_PROPERTY(bool filterArticle READ filterArticle WRITE setFilterArticle
                   NOTIFY filterArticleChanged)
    Q_PROPERTY(bool filterComponent READ filterComponent WRITE
                   setFilterComponent NOTIFY filterComponentChanged)
    Q_PROPERTY(bool filterMedia READ filterMedia WRITE setFilterMedia NOTIFY
                   filterMediaChanged)
    Q_PROPERTY(bool filterStyle READ filterStyle WRITE setFilterStyle NOTIFY
                   filterStyleChanged)

public:
    explicit CMSFilterProxyModel(QObject *parent = nullptr);

    bool filterArticle() const;
    void setFilterArticle(bool f);

    bool filterComponent() const;
    void setFilterComponent(bool f);

    bool filterStyle() const;
    void setFilterStyle(bool f);

    bool filterMedia() const;
    void setFilterMedia(bool f);

signals:
    void filterArticleChanged(bool f);
    void filterComponentChanged(bool f);
    void filterStyleChanged(bool f);
    void filterMediaChanged(bool f);

protected:
    bool filterAcceptsRow(int source_row,
                          const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left,
                  const QModelIndex &source_right) const override;

private:
    bool m_filterArticle = true;
    bool m_filterComponent = true;
    bool m_filterStyle = true;
    bool m_filterMedia = true;
};

#endif  // CMSFILTERPROXYMODEL_H
