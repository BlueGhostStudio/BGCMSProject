#ifndef RESOURCEDIALOG_H
#define RESOURCEDIALOG_H

#include "ui_resourcedialog.h"

class ResourceDialog : public QDialog, private Ui::ResourceDialog {
    Q_OBJECT

public:
    explicit ResourceDialog(QWidget* parent = nullptr);

    QVariant resource(int cateID = -1) const;
    void setResource(const QVariantMap& data);

    /*public slots:
        void postMedia();*/

public:
    void accept() override;

private:
    int m_resID = -1;
    int m_cateID = -1;
};

#endif  // RESOURCEDIALOG_H
