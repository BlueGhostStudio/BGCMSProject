#ifndef RESSUBWIN_H
#define RESSUBWIN_H

#include "ressubwinwidgetbase.h"
#include "subwinbase.h"

class ResSubWin : public SubWinBase {
    Q_OBJECT
public:
    ResSubWin(ResSubWinWidgetBase* widget, QWidget* parent = nullptr,
              Qt::WindowFlags flags = Qt::WindowFlags());

    ResSubWinWidgetBase* resWidget() const;

    int winID() const override;
};

#endif  // RESSUBWIN_H
