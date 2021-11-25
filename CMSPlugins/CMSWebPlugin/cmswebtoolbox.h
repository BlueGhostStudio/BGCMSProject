#ifndef CMSWEBTOOLBOX_H
#define CMSWEBTOOLBOX_H

#include "ui_cmswebtoolbox.h"

class CMSWebToolbox : public QGroupBox, private Ui::WebBrowerToolbox {
    Q_OBJECT

public:
    explicit CMSWebToolbox(QWidget* parent = nullptr);

signals:
    void openPreview();
    void openCurrentResource();
    void settings();
};

#endif  // CMSWEBTOOLBOX_H
