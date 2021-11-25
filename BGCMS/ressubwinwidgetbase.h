#ifndef RESSUBWINWIDGETBASE_H
#define RESSUBWINWIDGETBASE_H

#include <QWidget>
#include <QtCore>

//#include "subwinwidgetbase.h"

class ResSubWinWidgetBase : public QWidget {
    Q_OBJECT
    // Q_PROPERTY(int winID READ winID NOTIFY winIDChanged)

public:
    explicit ResSubWinWidgetBase(QWidget* parent = nullptr);

    int resID() const;
    virtual bool modified();

    /*signals:
        void winIDChanged(int id);*/
signals:
    void updateWindowTitle(const QString& title, const QIcon& icon);
    void closeWin();

public slots:
    virtual void openResource(const QVariantMap& resource);
    virtual void closeAboutDeleted();

protected slots:
    void setWindow();

protected:
    QVariantMap m_resource;
};

#endif  // RESSUBWINWIDGETBASE_H
