#include "snippetstoolbox.h"

SnippetsToolbox::SnippetsToolbox(QWidget* parent) : QGroupBox(parent) {
    setupUi(this);
    QObject::connect(ui_pbEdit, &QPushButton::clicked, this,
                     &SnippetsToolbox::editSnippets);
}
