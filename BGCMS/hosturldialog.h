#ifndef HOSTURLDIALOG_H
#define HOSTURLDIALOG_H

#include <QUrl>

#include "ui_hosturldialog.h"

class HostUrlDialog : public QDialog, private Ui::hostUrlDialog {
    Q_OBJECT

public:
    explicit HostUrlDialog(QWidget* parent = nullptr);

    QUrl url() const;
    QString urlString() const;
    void setUrl(const QString& url);
};

#endif  // HOSTURLDIALOG_H
