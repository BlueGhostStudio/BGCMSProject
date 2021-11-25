#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QtDebug>

#include "ui_logindialog.h"

class LoginDialog : public QDialog, private Ui::LoginDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog() { qDebug() << "logindlg destroy"; }

    QString token() const;
    void setToken(const QString& t);
};

#endif  // LOGINDIALOG_H
