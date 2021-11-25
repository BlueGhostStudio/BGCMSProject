#include "pluginsubwin.h"

#include <QCloseEvent>
#include <QIcon>
#include <QtDebug>

int PluginSubWin::lastPluginWinID = -2;

PluginSubWin::PluginSubWin(QWidget* widget, QWidget* parent,
                           Qt::WindowFlags flags)
    : SubWinBase(parent, flags) {
    widget->setParent(this);
    setWidget(widget);
    setSubWinTitle(widget->windowTitle(), widget->windowIcon());

    QObject::connect(widget, &QWidget::windowTitleChanged, this,
                     &PluginSubWin::setWindowTitle);
    QObject::connect(widget, &QWidget::windowIconChanged, this,
                     &PluginSubWin::setWindowIcon);

    m_pluginWinID = lastPluginWinID;
    lastPluginWinID--;
}

QWidget*
PluginSubWin::pluginWidget() const {
    return widget();
}

int
PluginSubWin::winID() const {
    return m_pluginWinID;
}

void
PluginSubWin::setDeleteWidgetOnClose(bool d) {
    m_deleteWidgetOnClose = d;
}

void
PluginSubWin::closeEvent(QCloseEvent* event) {
    if (!m_deleteWidgetOnClose) widget()->setParent(nullptr);
    event->accept();
}
