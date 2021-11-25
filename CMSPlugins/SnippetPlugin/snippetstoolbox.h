#ifndef SNIPPETSTOOLBOX_H
#define SNIPPETSTOOLBOX_H

#include "ui_snippetstoolbox.h"

class SnippetsToolbox : public QGroupBox, private Ui::SnippetsToolbox {
    Q_OBJECT

public:
    explicit SnippetsToolbox(QWidget* parent = nullptr);

signals:
    void editSnippets();
};

#endif  // SNIPPETSTOOLBOX_H
