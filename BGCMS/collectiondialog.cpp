#include "collectiondialog.h"

CollectionDialog::CollectionDialog(QWidget* parent) : QDialog(parent) {
    setupUi(this);
}

QVariant
CollectionDialog::collection() const {
    QString title(ui_leTitle->text());
    if (title.isEmpty()) title = "No Title";

    return QVariantMap(
        { { "title", title },
          { "alias", ui_leAlias->text() },
          { "desc", ui_pteDesc->toPlainText() },
          { "seq", ui_spbSeq->value() },
          { "id", m_collID },
          { "private", ui_cbPrivate->checkState() == Qt::Checked } });
}

void
CollectionDialog::setCollection(const QVariantMap& coll) {
    ui_leTitle->setText(coll["title"].toString());
    ui_leAlias->setText(coll["alias"].toString());
    ui_pteDesc->setPlainText(coll["desc"].toString());
    ui_spbSeq->setValue(coll["seq"].toInt());
    m_collID = coll["id"].toInt();
    ui_cbPrivate->setChecked(coll["private"].toBool());
}
