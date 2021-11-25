#ifndef SUBWINBASE_H
#define SUBWINBASE_H

#include <QMdiSubWindow>

class SubWinBase : public QMdiSubWindow {
    Q_OBJECT
public:
    SubWinBase(QWidget* parent = nullptr,
               Qt::WindowFlags flags = Qt::WindowFlags());

    virtual int winID() const;

protected slots:
    void setSubWinTitle(const QString& title, const QIcon& icon);
};

#endif  // SUBWINBASE_H
