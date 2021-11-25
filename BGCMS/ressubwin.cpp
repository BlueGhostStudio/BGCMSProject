#include "ressubwin.h"

ResSubWin::ResSubWin(ResSubWinWidgetBase* widget, QWidget* parent,
                     Qt::WindowFlags flags)
    : SubWinBase(parent, flags) {
    widget->setParent(this);
    setWidget(widget);
    QObject::connect(widget, &ResSubWinWidgetBase::updateWindowTitle, this,
                     &ResSubWin::setSubWinTitle);
    QObject::connect(widget, &ResSubWinWidgetBase::closeWin, this,
                     &ResSubWin::close);
}

ResSubWinWidgetBase*
ResSubWin::resWidget() const {
    return qobject_cast<ResSubWinWidgetBase*>(widget());
}

int
ResSubWin::winID() const {
    return resWidget()->resID();
}

/*void
ResSubWin::setSubWinTitle(const QString& title, const QIcon& icon) {
    QMdiSubWindow::setWindowTitle(title);
    setWindowIcon(icon);
}*/
