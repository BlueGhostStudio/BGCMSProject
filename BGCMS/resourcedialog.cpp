#include "resourcedialog.h"

#include <global.h>

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

ResourceDialog::ResourceDialog(QWidget* parent) : QDialog(parent) {
    setupUi(this);
    QObject::connect(
        bgType,
        QOverload<QAbstractButton*, bool>::of(&QButtonGroup::buttonToggled),
        this, [=](QAbstractButton* btn, bool checked) {
            qDebug() << btn->property("value") << checked;
            if (btn->property("value").toString() == "M" && checked)
                ui_stwProperty->setCurrentIndex(1);
            else
                ui_stwProperty->setCurrentIndex(0);
        });

    QObject::connect(ui_pbOpenImgFile, &QPushButton::clicked, this, [=]() {
        ui_leImgFile->setText(QFileDialog::getOpenFileName(
            this, tr("Media"),
            QStandardPaths::standardLocations(
                QStandardPaths::PicturesLocation)[0],
            "*.png *.jpg *.jpeg"));
    });
}

QVariant
ResourceDialog::resource(int cateID) const {
    QVariantMap resData;
    /*qDebug() << "---checkedID--->"
             << bgType->checkedButton()->property("value");*/
    QString type = bgType->checkedButton()->property("value").toString();
    resData["cateid"] = cateID == -1 ? m_cateID : cateID;
    resData["type"] = type;
    resData["title"] = leTitle->text();
    resData["alias"] = leAlias->text();
    resData["id"] = m_resID;
    resData["hide"] = ui_cbHide->checkState() == Qt::Checked;
    resData["private"] = ui_cbPrivate->isChecked();

    if (type != 'M') {
        resData["desc"] = pteDesc->toPlainText();
        resData["seq"] = spbSeq->value();
    } else if (m_resID == -1)
        resData["content"] = ui_leImgFile->text();

    return resData;
}

void
ResourceDialog::setResource(const QVariantMap& data) {
    leTitle->setText(data["title"].toString());
    leAlias->setText(data["alias"].toString());
    pteDesc->setPlainText(data["desc"].toString());
    spbSeq->setValue(data["seq"].toInt());
    ui_cbHide->setChecked(data["hide"].toBool());
    ui_cbPrivate->setChecked(data["private"].toBool());
    QString type = data["type"].toString();

    m_resID = data["id"].toInt();
    m_cateID = data["cateid"].toInt();

    if (type == "A")
        rbArticle->setChecked(true);
    else if (type == "C")
        rbComponent->setChecked(true);
    else if (type == "S")
        rbStyle->setChecked(true);
    else if (type == "M") {
        ui_leImgFile->setText(data["content"].toString());
        ui_imgPage->setEnabled(false);
        rbMedia->setChecked(true);
        //        rbMedia->setEnabled(false);
        rbArticle->setEnabled(false);
        rbComponent->setEnabled(false);
        rbStyle->setEnabled(false);

        /*bgmrpcClient.callMethod("CMS", "resourceContent", { m_resID })
            ->then([=](const QVariant& ret) {
                qDebug() << "------>" << ret;
                ui_leImgFile->setText(ret.toList()[0].toString());
            });*/
    }

    rbMedia->setEnabled(false);
}

void
ResourceDialog::accept() {
    if (bgType->checkedButton()->property("value").toString() != "M" ||
        m_resID != -1 || ui_leImgFile->text().length() > 0)
        QDialog::accept();
    else
        QMessageBox::warning(
            this, tr("Media file"),
            tr("You need to specify the file to upload the image"));
}

/*void
ResourceDialog::postMedia() {
    QStringList sp =
        QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QString fileName = QFileDialog::getOpenFileName(this, "Media File", sp[0]);

    CallGraph::start("getFile")
        ->nodes("getFile",
                [=](CallGraph* cg, const QVariant& fileName) {
                    bgmrpcClient.callMethod("CMSRes", "requestPostImg", {})
                        ->then([=](const QVariant& ret) {
                            qDebug() << ret;
                            ui_mediaFile->setText(ret.toList()[0].toString());
                            cg->to("readImgData", fileName.toString());
                        });
                })
        ->nodes("readImgData",
                [=](CallGraph* cg, const QVariant& fileName) {
                    QFile mediaFile(fileName.toString());
                    if (mediaFile.open(QIODevice::ReadOnly))
                        cg->to("postImgData", mediaFile.readAll());
                    else
                        cg->toFinal();
                })
        ->nodes("postImgData",
                [=](CallGraph* cg, const QVariant& data) {
                    QByteArray ba = data.toByteArray();
                    QByteArray part = ba.left(1024).toBase64();
                    // ba.remove(0, 1024);
                    // if (ba.length() > 0)
                    bgmrpcClient
                        .callMethod("CMSRes", "writeImgData",
                                    { ui_mediaFile->text(), part })
                        ->then([=](const QVariant&) {
                            if (ba.length() > 1024)
                                cg->to("postImgData", ba.mid(1024));
                            else
                                cg->to("end");
                        });
                })
        ->nodes("end",
                [=](CallGraph* cg, const QVariant&) {
                    bgmrpcClient
                        .callMethod("CMSRes", "writeImgDataEnd",
                                    { ui_mediaFile->text() })
                        ->then([=](const QVariant&) { cg->toFinal(); });
                })
        ->exec(fileName);
}*/
