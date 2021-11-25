#include "categorydialog.h"

CategoryDialog::CategoryDialog(QWidget* parent) : QDialog(parent) {
    setupUi(this);
}

QVariant
CategoryDialog::category() const {
    QString title(ui_leTitle->text());
    if (title.isEmpty()) title = "No Title";

    return QVariantMap({ { "title", title },
                         { "alias", ui_leAlias->text() },
                         { "seq", ui_spbSeq->value() },
                         { "hide", ui_cbHide->checkState() == Qt::Checked },
                         { "id", m_cateID },
                         { "cid", m_collID },
                         { "private", ui_cbPrivate->isChecked() } });
}

void
CategoryDialog::setCategory(const QVariantMap& cate) {
    ui_leTitle->setText(cate["title"].toString());
    ui_leAlias->setText(cate["alias"].toString());
    ui_spbSeq->setValue(cate["seq"].toInt());
    ui_cbHide->setChecked(cate["hide"].toBool());
    m_cateID = cate["id"].toInt();
    m_collID = cate["cid"].toInt();
    ui_cbPrivate->setChecked(cate["private"].toBool());
}
