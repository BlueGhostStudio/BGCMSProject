#include "cmswebtoolbox.h"

CMSWebToolbox::CMSWebToolbox(QWidget* parent) : QGroupBox(parent) {
    setupUi(this);
    QObject::connect(ui_pbPreview, &QPushButton::clicked, this,
                     &CMSWebToolbox::openPreview);
    QObject::connect(ui_pbOpenResource, &QPushButton::clicked, this,
                     &CMSWebToolbox::openCurrentResource);
    QObject::connect(ui_pbSettings, &QPushButton::clicked, this,
                     &CMSWebToolbox::settings);
}
