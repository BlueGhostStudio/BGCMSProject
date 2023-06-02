#include "hosturldialog.h"

HostUrlDialog::HostUrlDialog(QWidget* parent) : QDialog(parent) {
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);

    setWindowTitle(tr("Host Url"));
}

QUrl
HostUrlDialog::url() const {
    return QUrl(ui_leUrl->text());
}

QString
HostUrlDialog::urlString() const {
    return ui_leUrl->text();
}

void
HostUrlDialog::setUrl(const QString& url) {
    ui_leUrl->setText(url);
}
