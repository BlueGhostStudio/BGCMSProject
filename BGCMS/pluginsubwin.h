#ifndef PLUGINSUBWIN_H
#define PLUGINSUBWIN_H

#include "subwinbase.h"

class PluginSubWin : public SubWinBase {
    Q_OBJECT
public:
    PluginSubWin(QWidget* widget, QWidget* parent = nullptr,
                 Qt::WindowFlags flags = Qt::WindowFlags());

    QWidget* pluginWidget() const;

    int winID() const override;
    void setDeleteWidgetOnClose(bool d);

    virtual void closeEvent(QCloseEvent* event) override;

private:
    bool m_deleteWidgetOnClose = true;
    int m_pluginWinID;
    static int lastPluginWinID;
};

#endif  // PLUGINSUBWIN_H
