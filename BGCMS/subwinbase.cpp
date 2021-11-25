#include "subwinbase.h"

SubWinBase::SubWinBase(QWidget* parent, Qt::WindowFlags flags)
    : QMdiSubWindow(parent, flags) {
    setAttribute(Qt::WA_DeleteOnClose, true);
}

int
SubWinBase::winID() const {
    return -2;
}

void
SubWinBase::setSubWinTitle(const QString& title, const QIcon& icon) {
    setWindowTitle(title);
    setWindowIcon(icon);
}
