#include "logindialog.h"

#include <QPushButton>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setupUi(this);
    QPushButton* btnNewRegister =
        ui_buttonBox->addButton(tr("注册新用户"), QDialogButtonBox::AcceptRole);
    QObject::connect(btnNewRegister, &QPushButton::clicked, this,
                     [=]() { done(2); });
    QObject::connect(ui_buttonBox->button(QDialogButtonBox::Ok),
                     &QPushButton::clicked, this, [=]() { done(1); });
}

QString
LoginDialog::token() const {
    return ui_leToken->text();
}

void
LoginDialog::setToken(const QString& t) {
    ui_leToken->setText(t);
}
