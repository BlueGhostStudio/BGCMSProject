#ifndef CATEGORYDIALOG_H
#define CATEGORYDIALOG_H

#include "ui_categorydialog.h"

class CategoryDialog : public QDialog, private Ui::CategoryDialog {
    Q_OBJECT

public:
    explicit CategoryDialog(QWidget* parent = nullptr);

    QVariant category() const;
    void setCategory(const QVariantMap& cate);

private:
    int m_cateID = -1;
    int m_collID = -1;
};

#endif  // CATEGORYDIALOG_H
