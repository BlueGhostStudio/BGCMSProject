/*****************************************************************************
 * imageviewer.cpp
 *
 * Created: 2022/2/27 2022 by blueghost
 *
 * Copyright 2022 blueghost. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#include "imageviewer.h"

#include <QGraphicsScene>

ImageViewer::ImageViewer(QWidget* parent) : QMainWindow(parent) {
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    ui_gvImageViewer->setScene(new QGraphicsScene);

    m_loadProgressBar = new QProgressBar;
    ui_statusbar->addPermanentWidget(m_loadProgressBar);
    m_loadProgressBar->hide();

    QObject::connect(action_fitInView, &QAction::triggered, this, [=]() {
        ui_gvImageViewer->fitInView(ui_gvImageViewer->sceneRect(),
                                    Qt::KeepAspectRatio);
    });
    QObject::connect(action_zoom1_1, &QAction::triggered, this,
                     [=]() { ui_gvImageViewer->setTransform(QTransform()); });
}

void
ImageViewer::loadImage(CMSMediaApi* mediaApi, const QVariantMap& node) {
    show();
    CallGraph::start("nodePath", this)
        ->nodes("nodePath",
                [=](CallGraph* cg, const QVariant& data) {
                    mediaApi->cmsApi()->nodePath(node["id"], cg, "setWinTitle");
                })
        ->nodes("setWinTitle",
                [=](CallGraph* cg, const QVariant& data) {
                    setWindowTitle(tr("BGCMS") + " - " +
                                   data.toMap()["str"].toString());
                    cg->to("getNode");
                })
        ->nodes("getNode",
                [=](CallGraph* cg, const QVariant&) {
                    mediaApi->cmsApi()->node(node["id"], cg, "getMedia",
                                             "error");
                })
        ->nodes("getMedia",
                [=](CallGraph* cg, const QVariant& data) {
                    QString content = data.toMap()["content"].toString();
                    if (content.contains(QRegularExpression("^res:")))
                        mediaApi->getMedia(content, cg, "view", "error",
                                           "progressBegin", "progress");
                    else if (content.contains(QRegularExpression("^link:")))
                        mediaApi->getImgFromUrl(content, cg, "view", "error");
                    else
                        cg->to("error", tr("unknow open"));
                })
        ->nodes(
            "progressBegin",
            [=](CallGraph* cg, const QVariant& data) {
                m_loadProgressBar->show();
                m_loadProgressBar->setValue(0);
                m_loadProgressBar->setMaximum(data.toInt());
            },
            "progress",
            [=](CallGraph* cg, const QVariant& data) {
                m_loadProgressBar->setValue(data.toInt());
            })
        ->nodes("view",
                [=](CallGraph* cg, const QVariant& data) {
                    m_loadProgressBar->hide();
                    QPixmap image;
                    image.loadFromData(data.toByteArray());
                    ui_gvImageViewer->scene()->addPixmap(image);
                    ui_gvImageViewer->fitInView(ui_gvImageViewer->sceneRect(),
                                                Qt::KeepAspectRatio);
                    cg->toFinal();
                })
        ->nodes("error",
                [=](CallGraph* cg, const QVariant& data) {
                    qDebug() << "-----error-----" << data;
                    cg->toFinal();
                })
        ->exec();
}
