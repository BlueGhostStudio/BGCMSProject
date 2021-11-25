#include "cmssubwin.h"

#include <QCloseEvent>
#include <QtDebug>

CMSSubWin::CMSSubWin() {
    setSubWinTitle(tr("CMS"), QIcon(":/imgs/home.png"));
    setWidget(&ui_cmsWidget);
    QObject::connect(&ui_cmsWidget, &CMSWidget::titleChanged, this,
                     &CMSSubWin::setWindowTitle);
}

int
CMSSubWin::winID() const {
    return -1;
}

CMSWidget*
CMSSubWin::cmsWidget() {
    return &ui_cmsWidget;
}

void
CMSSubWin::closeEvent(QCloseEvent* event) {
    event->ignore();
}
