#include "cmsbrower.h"

#include <QContextMenuEvent>
#include <QStandardItemModel>
#include <QtDebug>

#include "global.h"

CMSBrower::CMSBrower(QWidget* parent) : QListView(parent) {}

void
CMSBrower::currentChanged(const QModelIndex& current,
                          const QModelIndex& /*previous*/) {
    emit hasCurrent(current.row() != -1);
}

void
CMSBrower::contextMenuEvent(QContextMenuEvent* event) {
    QPoint globalPos = event->globalPos();
    if (indexAt(event->pos()).isValid())
        emit entryMenu(globalPos);
    else
        emit cmsMenu(globalPos);
}

void
CMSBrower::reset() {
    QListView::reset();
    setCurrentIndex(model()->index(-1, -1));
    emit hasCurrent(false);
}
