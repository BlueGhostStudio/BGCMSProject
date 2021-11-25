#ifndef COLLECTIONDIALOG_H
#define COLLECTIONDIALOG_H

//#include "ui_newcollectiondialog.h"
#include "ui_collectiondialog.h"

class CollectionDialog : public QDialog, private Ui::CollectionDialog {
    Q_OBJECT

public:
    explicit CollectionDialog(QWidget* parent = nullptr);
    QVariant collection() const;
    void setCollection(const QVariantMap& coll);

private:
    int m_collID = -1;
};

#endif  // COLLECTIONDIALOG_H
