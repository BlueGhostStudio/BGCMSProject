/*****************************************************************************
 * cmsmediaapi.cpp
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
#include "cmsmediaapi.h"

#include <QNetworkReply>

CMSMediaApi::CMSMediaApi(QObject* parent) : QObject(parent) {}

void
CMSMediaApi::setCMSApi(CMSApi* api) {
    m_cmsApi = api;
}

CMSApi*
CMSMediaApi::cmsApi() const {
    return m_cmsApi;
}

void
CMSMediaApi::getMedia(const QString& content, CallGraph* pcg,
                      const QString& to, const QString& error,
                      const QString& progressBegin, const QString& progress) {
    if (content.contains(QRegularExpression("^res:"))) {
        QString fileName(content);
        fileName.remove(QRegularExpression("^res:"));
        QByteArray* mediaData = new QByteArray;

        CallGraph::start("requestGetMedia", this)
            ->nodes(
                "requestGetMedia",
                [=](CallGraph* cg, const QVariant&) {
                    m_cmsApi
                        ->callMethod(m_cmsApi->CMSObjID("media"),
                                     "requestGetMedia", { fileName })
                        ->then([=](const QVariant& ret) {
                            QVariantList retList = ret.toList();
                            QVariant ret0 = retList[0];
                            if (ret0.typeId() == QMetaType::Bool &&
                                ret0.toBool() == false) {
                                if (pcg/*.isNull()*/ && !error.isEmpty())
                                    pcg->to(error, "can't request Get Media");

                                cg->to("end");
                            } else {
                                if (pcg/*.isNull()*/ && !progressBegin.isEmpty())
                                    pcg->to(progressBegin, retList[1]);
                                cg->to("getMedia",
                                       QVariantList({ ret0, 0, 4096 }));
                            }
                        });
                })
            ->nodes("getMedia",
                    [=](CallGraph* cg, const QVariant& data) {
                        QVariantList dataList = data.toList();
                        m_cmsApi
                            ->callMethod(m_cmsApi->CMSObjID("media"),
                                         "readMediaData", dataList)
                            ->then([=](const QVariant& ret) {
                                QVariantList retList = ret.toList();
                                QVariantMap ret0 = retList[0].toMap();

                                *mediaData += QByteArray::fromBase64(
                                    ret0["data"].toByteArray());
                                int pos = ret0["pos"].toInt();
                                if (pos == -1) {
                                    if (pcg/*.isNull()*/ && !to.isEmpty()) {
                                        if (!progress.isEmpty())
                                            pcg->to(progress,
                                                    mediaData->length());

                                        pcg->to(to, *mediaData);
                                    }

                                    cg->to("end");
                                } else {
                                    if (pcg/*.isNull()*/ && !progress.isEmpty())
                                        pcg->to(progress, pos);
                                    if (pcg/*.isNull()*/)
                                        cg->to("getMedia",
                                               QVariantList(
                                                   { dataList[0], pos, 4096 }));
                                    else
                                        cg->to("end");
                                }
                            });
                    })
            ->nodes("end",
                    [=](CallGraph* cg, const QVariant&) {
                        delete mediaData;
                        cg->toFinal();
                    })
            ->exec();
    } else if (pcg/*.isNull()*/ && !error.isEmpty())
        pcg->to(error, tr("is not resource"));
}

void
CMSMediaApi::getImgUrl(const QString& content, CallGraph* pcg,
                       const QString& to, const QString& error) {
    if (content.contains(QRegularExpression("^link:"))) {
        if (pcg/*.isNull()*/ && !to.isEmpty()) pcg->to(to, content.mid(5));
    } else if (content.contains(QRegularExpression("^res:"))) {
        m_cmsApi
            ->callMethod(m_cmsApi->CMSObjID("media"), "mediaURL",
                         { content.mid(4) })
            ->then([=](const QVariant& ret) {
                if (pcg/*.isNull()*/ && !to.isEmpty())
                    pcg->to(to, ret.toList()[0]);
            });
    } else if (pcg/*.isNull()*/ && !error.isEmpty())
        pcg->to(error, tr("Unknow type"));
}

void
CMSMediaApi::getImgFromUrl(const QString& content, CallGraph* pcg,
                           const QString& to, const QString& error) {
    if (content.contains(QRegularExpression("^link:"))) {
        QNetworkAccessManager* manager = new QNetworkAccessManager;
        QUrl url(QString(content).remove(QRegularExpression("^link:")));
        QObject::connect(manager, &QNetworkAccessManager::finished, this,
                         [=](QNetworkReply* reply) {
                             qDebug() << "finisied";
                             if (reply->error() == QNetworkReply::NoError) {
                                 if (pcg/*.isNull()*/ && !to.isEmpty())
                                     pcg->to(to, reply->readAll());
                             } else if (pcg/*.isNull()*/ && !error.isEmpty())
                                 pcg->to(error, tr("get img fail"));

                             manager->deleteLater();
                         });
        manager->get(QNetworkRequest(url));
    } else if (pcg/*.isNull()*/ && !error.isEmpty())
        pcg->to(error, tr("is not Url"));
}

void
CMSMediaApi::postMedia(const QVariant& pNode, const QString& fileName,
                       CallGraph* pcg, const QString& to,
                       const QString& error, const QString& progressBegin,
                       const QString& progress) {
    QString contentType = fileName.section('.', -1).toLower();
    CallGraph::start("readImgData", this)
        ->nodes("readImgData",
                [=](CallGraph* cg, const QVariant&) {
                    QFile imgFile(fileName);
                    if (imgFile.exists() && imgFile.open(QIODevice::ReadOnly)) {
                        QByteArray imgData = imgFile.readAll();
                        imgFile.close();
                        cg->to("requestPostMedia", imgData);
                    } else {
                        if (pcg/*.isNull()*/ && !error.isEmpty())
                            pcg->to(error, tr("file no exist."));

                        cg->toFinal();
                    }
                })
        ->nodes("requestPostMedia",
                [=](CallGraph* cg, const QVariant& data) {
                    m_cmsApi
                        ->callMethod(m_cmsApi->CMSObjID("media"),
                                     "requestPostMedia", { contentType })
                        ->then([=](const QVariant& ret) {
                            QVariantList retList = ret.toList();
                            QVariant ret0 = retList[0];

                            if (pcg/*.isNull()*/ && !progressBegin.isEmpty())
                                pcg->to(progressBegin,
                                        data.toByteArray().size());

                            cg->to("postMedia", QVariantList({ ret0, data }));
                        });
                })
        ->nodes(
            "postMedia",
            [=](CallGraph* cg, const QVariant& data) {
                QVariantList list = data.toList();
                QVariant id = list[0];
                QByteArray imgData = list[1].toByteArray();
                QByteArray partData = imgData.left(65536);

                int postingSize = partData.size();

                if (postingSize > 0) {
                    m_cmsApi
                        ->callMethod(m_cmsApi->CMSObjID("media"),
                                     "writeMediaData",
                                     { id, partData.toBase64() })
                        ->then([=](const QVariant&) {
                            if (pcg/*.isNull()*/ && !progress.isEmpty())
                                pcg->to(progress, postingSize);

                            cg->to("postMedia",
                                   QVariantList({ id, imgData.mid(65536) }));
                        });
                } else {
                    // cg->to("newMediaNode", id);
                    CallGraph::start("newMediaNode", this)
                        ->nodes(
                            "newMediaNode",
                            [=](CallGraph* NMNCG, const QVariant&) {
                                QVariantMap newNode;
                                newNode["name"] = id;
                                newNode["contentType"] = contentType;
                                newNode["content"] = "res:" + id.toString();
                                newNode["title"] =
                                    QFileInfo(fileName).fileName();
                                newNode["type"] = "R";
                                m_cmsApi->addNode(pNode, newNode, NMNCG,
                                                  "finished", "error");
                            })
                        ->nodes(
                            "finished",
                            [=](CallGraph* NMNCG, const QVariant&) {
                                cg->to("save", id);
                            },
                            "error",
                            [=](CallGraph* cg, const QVariant&) {
                                cg->to("error", id);
                            })
                        ->exec();
                }
            })
        ->nodes(
            "save",
            [=](CallGraph* cg, const QVariant& data) {
                qDebug() << "---- save ----" << data;
                m_cmsApi
                    ->callMethod(m_cmsApi->CMSObjID("media"),
                                 "writeMediaDataEnd", { data })
                    ->then([=](const QVariant& ret) {
                        if (pcg/*.isNull()*/ && !to.isEmpty()) pcg->to(to);
                        cg->toFinal();
                    });
            },
            "error",
            [=](CallGraph* cg, const QVariant& data) {
                qDebug() << "------> error ----->" << data;
                m_cmsApi
                    ->callMethod(m_cmsApi->CMSObjID("media"), "close", { data })
                    ->then([=](const QVariant& ret) {
                        if (pcg/*.isNull()*/ && !error.isEmpty()) pcg->to(error);
                        cg->toFinal();
                    });
            })
        ->exec();
}

void
CMSMediaApi::removeMedia(const QString& id, CallGraph* pcg,
                         const QString& to, const QString& error) {
    m_cmsApi->callMethod(m_cmsApi->CMSObjID("media"), "removeMedia", { id })
        ->then([=](const QVariant& ret) {
            bool ok = ret.toList()[0].toBool();
            if (ok) {
                if (pcg/*.isNull()*/ && !to.isEmpty()) pcg->to(to);
            } else if (pcg/*.isNull()*/ && !error.isEmpty())
                pcg->to(error, tr("Can not remove Media"));
        });
}
