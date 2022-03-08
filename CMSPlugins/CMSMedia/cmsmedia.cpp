/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/2/26 2022 by blueghost
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
#include "cmsmedia.h"

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressDialog>

CMSMedia::CMSMedia(QObject* parent) : QObject(parent) {}

pluginUI
CMSMedia::initial(CMSApi* api, CMSBrowserBase* browser, const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    if (!m_settings.contains("contentType/image"))
        m_settings.setValue("contentType/image", QStringList() << "png"
                                                               << "jpg"
                                                               << "jpeg"
                                                               << "gif");
    if (!m_settings.contains("contentType/media"))
        m_settings.setValue("contentType/media", QStringList() << "mp3"
                                                               << "mp4");

    m_mediaApi.setCMSApi(api);

    QObject::connect(m_cmsBrowser, SIGNAL(nodeItemDoubleClicked(QVariantMap)),
                     this, SLOT(openMedia(QVariantMap)));

    QObject::connect(
        m_mediaApi.cmsApi(), &CMSApi::resourceNodeRemoved, this,
        [=](const QVariant& node, const QString& contentType,
            const QString& content) {
            /*qDebug() << "----media----" << node << contentType
                     << content;*/

            if (m_settings.value("contentType/image")
                    .toList()
                    .contains(contentType) ||
                m_settings.value("contentType/media")
                    .toList()
                    .contains(contentType)) {
                if (content.contains(QRegularExpression("^res:")))
                    CallGraph::start("removeMedia", this)
                        ->nodes("removeMedia",
                                [=](CallGraph* cg, const QVariant&) {
                                    m_mediaApi.removeMedia(
                                        QString(content).remove(
                                            QRegularExpression("^res:")),
                                        cg, "ok", "error");
                                })
                        ->nodes(
                            "ok",
                            [=](CallGraph* cg, const QVariant&) {
                                qDebug() << "remove Media ok";
                            },
                            "error",
                            [=](CallGraph* cg, const QVariant&) {
                                qDebug() << "remove Media error";
                            })
                        ->exec();
            }
        });

    ui.newNodeMenu->addAction(tr("Media"), this, &CMSMedia::postMedia);

    ui.settingMenu->addAction(tr("Media Types"), this, [=]() {
        QString typesText;
        bool begin = true;
        foreach (const QVariant& type,
                 m_settings.value("contentType/media").toList()) {
            if (begin)
                begin = false;
            else
                typesText += ',';

            typesText += type.toString();
        }

        bool ok;
        typesText = QInputDialog::getText(
            qApp->activeWindow(), tr("Media Content Types"), tr("Types"),
            QLineEdit::Normal, typesText, &ok);

        if (ok) {
            m_settings.setValue("contentType/media", typesText.split(","));
        }
    });

    m_actionCopyImageUrl =
        new QAction(QIcon(":/copyImgUrl.png"), tr("Copy Media Url"));
    m_actionCopyImageUrl->setEnabled(false);
    ui.pluginsToolbar->addAction(m_actionCopyImageUrl);
    ui.pluginsMenu->addAction(m_actionCopyImageUrl);

    QObject::connect(m_cmsBrowser,
                     SIGNAL(selectedNodeChanged(const QItemSelection&,
                                                const QItemSelection&)),
                     this, SLOT(setActionEnabled()));

    QClipboard* clipboard = QGuiApplication::clipboard();

    QObject::connect(m_actionCopyImageUrl, &QAction::triggered, this, [=]() {
        QVariantMap node = m_cmsBrowser->currentNode();
        CallGraph::start("getNodeContent", this)
            ->nodes("getNodeContent",
                    [=](QPointer<CallGraph> cg, const QVariant&) {
                        m_api->node(node["id"], cg, "copyUrl", "error");
                    })
            ->nodes(
                "copyUrl",
                [=](QPointer<CallGraph> cg, const QVariant& data) {
                    QString content = data.toMap()["content"].toString();
                    //                        m_mediaApi.getImgUrl(content,)
                    CallGraph::start("getUrl", this)
                        ->nodes("getUrl",
                                [=](QPointer<CallGraph> cg, const QVariant&) {
                                    m_mediaApi.getImgUrl(content, cg, "copyUrl",
                                                         "error");
                                })
                        ->nodes(
                            "copyUrl",
                            [=](QPointer<CallGraph> cg, const QVariant& data) {
                                qDebug() << "-----" << data;
                                clipboard->setText(data.toString());
                            },
                            "error",
                            [=](QPointer<CallGraph> cg, const QVariant& data) {
                                qDebug() << data;
                            })
                        ->exec();
                })
            ->exec();
    });

    return pluginUI();
}

void
CMSMedia::openMedia(const QVariantMap& node) {
    ImageViewer* viewer = new ImageViewer(qApp->activeWindow());
    if (node["type"] == "R") {
        QString contentType = node["contentType"].toString();
        if (m_settings.value("contentType/image")
                .toList()
                .contains(contentType)) {
            viewer->loadImage(&m_mediaApi, node);
        } else if (m_settings.value("contentType/media")
                       .toList()
                       .contains(contentType)) {
            qDebug() << "open Media";
        }
    }
}

void
CMSMedia::postMedia() {
    QString fileName = QFileDialog::getOpenFileName(
        qApp->activeWindow(), tr("Post Media"),
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        tr("Image(*.png *.jpg *.jpeg *.bmp *.xpm);;Audio(*.mp3 *.ogg "
           "*.flac"));

    if (!fileName.isEmpty()) {
        QProgressDialog* postProgress =
            new QProgressDialog(qApp->activeWindow());
        postProgress->setAttribute(Qt::WA_DeleteOnClose);
        postProgress->setCancelButton(nullptr);
        postProgress->setWindowTitle(tr("Posting Media"));
        postProgress->setLabelText(fileName);
        postProgress->setModal(true);
        postProgress->hide();

        CallGraph::start("postMedia", this)
            ->nodes("postMedia",
                    [=](CallGraph* cg, const QVariant&) {
                        m_mediaApi.postMedia(m_cmsBrowser->listNode(), fileName,
                                             cg, "posted", "error",
                                             "progressBegin", "progress");
                    })
            ->nodes(
                "progressBegin",
                [=](CallGraph* cg, const QVariant& data) {
                    postProgress->setMaximum(data.toInt());
                    postProgress->setValue(0);
                    postProgress->show();
                },
                "progress",
                [=](CallGraph* cg, const QVariant& data) {
                    postProgress->setValue(postProgress->value() +
                                           data.toInt());
                })
            ->nodes(
                "posted",
                [=](CallGraph* cg, const QVariant&) {
                    postProgress->close();
                    cg->toFinal();
                },
                "error",
                [=](CallGraph* cg, const QVariant& data) { cg->toFinal(); })
            ->exec();
    }
}

void
CMSMedia::setActionEnabled() {
    if (m_cmsBrowser->selectionModel()->hasSelection()) {
        int row = m_cmsBrowser->currentIndex().row();
        QVariantMap node = m_cmsBrowser->currentNode();
        QString contentType = node["contentType"].toString();
        m_actionCopyImageUrl->setEnabled(m_settings.value("contentType/media")
                                             .toList()
                                             .contains(contentType) ||
                                         m_settings.value("contentType/image")
                                             .toList()
                                             .contains(contentType));
    } else
        m_actionCopyImageUrl->setEnabled(false);
}
