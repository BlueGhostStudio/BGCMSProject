#ifndef CMSBROWER_H
#define CMSBROWER_H

#include <QListView>

class CMSBrower : public QListView {
    Q_OBJECT
public:
    CMSBrower(QWidget* parent = nullptr);

signals:
    void hasCurrent(bool c);

protected slots:
    void currentChanged(const QModelIndex& current,
                        const QModelIndex& previous);

signals:
    void cmsMenu(const QPoint& pos);
    void entryMenu(const QPoint& pos);

protected:
    void contextMenuEvent(QContextMenuEvent* event);

public slots:
    void reset();
};

#endif  // CMSBROWER_H
