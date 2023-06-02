/*****************************************************************************
 * cmsmediaapi.h
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
#ifndef CMSMEDIAAPI_H
#define CMSMEDIAAPI_H

#include <QNetworkAccessManager>
#include <QObject>

#include "../../BGCMS/cmsapi.h"

class CMSMediaApi : public QObject {
    Q_OBJECT
public:
    explicit CMSMediaApi(QObject* parent = nullptr);

    void setCMSApi(CMSApi* api);
    CMSApi* cmsApi() const;
    void getMedia(const QString& content, CallGraph* pcg,
                  const QString& to, const QString& error,
                  const QString& progressBegin, const QString& progress);
    void getImgUrl(const QString& content, CallGraph* pcg,
                   const QString& to, const QString& error);
    void getImgFromUrl(const QString& content, CallGraph* pcg,
                       const QString& to, const QString& error);
    void postMedia(const QVariant& pNode, const QString& fileName,
                   CallGraph* pcg, const QString& to,
                   const QString& error, const QString& progressBegin,
                   const QString& progress);
    void removeMedia(const QString& id, CallGraph* pcg,
                     const QString& to, const QString& error);

signals:

private:
    CMSApi* m_cmsApi;
};

#endif  // CMSMEDIAAPI_H
