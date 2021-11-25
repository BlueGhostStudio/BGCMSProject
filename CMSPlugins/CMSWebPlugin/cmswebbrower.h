#ifndef CMSWEBBROWER_H
#define CMSWEBBROWER_H

#include <QWebEngineView>

class CMSWebBrower : public QWebEngineView {
    Q_OBJECT

public:
    CMSWebBrower(QWidget* parent = nullptr);
};

#endif  // CMSWEBBROWER_H
