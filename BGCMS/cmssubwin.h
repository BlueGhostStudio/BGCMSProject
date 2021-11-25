#ifndef CMSSUBWIN_H
#define CMSSUBWIN_H

#include <QMdiSubWindow>

#include "cmswidget.h"
#include "subwinbase.h"

class CMSSubWin : public SubWinBase {
    Q_OBJECT
public:
    CMSSubWin();

    int winID() const override;
    CMSWidget* cmsWidget();

protected:
    void closeEvent(QCloseEvent* closeEvent) override;

protected:
    CMSWidget ui_cmsWidget;
};

#endif  // CMSSUBWIN_H
