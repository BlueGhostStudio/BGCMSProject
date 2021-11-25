#ifndef IMGVIEWER_H
#define IMGVIEWER_H

#include "ressubwinwidgetbase.h"
#include "ui_imgviewer.h"

class ImgViewer : public ResSubWinWidgetBase, private Ui::ImgViewer {
    Q_OBJECT

public:
    explicit ImgViewer(QWidget* parent = nullptr);

public slots:
    void openResource(const QVariantMap& resource) override;

private:
    QByteArray m_imgData;
};

#endif  // IMGVIEWER_H
