#include "imgviewer.h"

#include <QGraphicsScene>
#include <QProgressDialog>
#include <QtDebug>

#include "global.h"

ImgViewer::ImgViewer(QWidget* parent) : ResSubWinWidgetBase(parent) {
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, true);
    ui_gvImage->setScene(new QGraphicsScene);
}

void
ImgViewer::openResource(const QVariantMap& resource) {
    ResSubWinWidgetBase::openResource(resource);

    /*QGraphicsScene* scene = new QGraphicsScene(this);
    ui_gvImage->setScene(scene);
    scene->addText("this is a test");*/

    QProgressDialog* postImgProgress = new QProgressDialog(this);
    postImgProgress->setWindowTitle(tr("Get Image"));
    postImgProgress->setLabelText(tr("Get..."));
    postImgProgress->setWindowModality(Qt::ApplicationModal);
    postImgProgress->setCancelButton(nullptr);
    postImgProgress->close();

    CallGraph::start("getImgID")
        ->nodes("getImgID",
                [=](CallGraph* cg, const QVariant&) {
                    bgmrpcClient
                        .callMethod("CMS", "resourceContent",
                                    { m_resource["id"] })
                        ->then([=](const QVariant& ret) {
                            cg->to("requestImg",
                                   ret.toList()[0].toMap()["content"]);
                        });
                })
        ->nodes(
            "requestImg",
            [=](CallGraph* cg, const QVariant& data) {
                bgmrpcClient.callMethod("CMSRes", "requestGetImg", { data })
                    ->then([=](const QVariant& ret) {
                        QVariantList list = ret.toList();
                        QVariant ret0 = list[0];
                        if (ret0.type() == QVariant::Bool &&
                            ret0.toBool() == false)
                            cg->toFinal();
                        else {
                            postImgProgress->setValue(0);
                            postImgProgress->setMaximum(list[1].toInt());
                            postImgProgress->open();
                            cg->to("getImg", QVariantList({ ret0, 0, 65536 }));
                        }
                    });
            })
        ->nodes("getImg",
                [=](CallGraph* cg, const QVariant& data) {
                    QVariantList list = data.toList();
                    bgmrpcClient
                        .callMethod("CMSRes", "readImgData",
                                    { list[0], list[1], list[2] })
                        ->then([=](const QVariant& ret) {
                            QVariantMap ret0 = ret.toList()[0].toMap();
                            m_imgData += QByteArray::fromBase64(
                                ret0["data"].toByteArray());
                            postImgProgress->setValue(m_imgData.length());
                            int pos = ret0["pos"].toInt();
                            if (pos == -1)
                                cg->to("end");
                            else
                                cg->to("getImg",
                                       QVariantList({ list[0], pos, 65536 }));
                        });
                })
        ->nodes("end",
                [=](CallGraph* cg, const QVariant&) {
                    ui_gvImage->scene()->addPixmap(
                        QPixmap::fromImage(QImage::fromData(m_imgData)));
                    delete postImgProgress;
                    cg->toFinal();
                })
        ->exec();
}
