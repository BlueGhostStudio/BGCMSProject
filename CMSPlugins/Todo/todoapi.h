/*****************************************************************************
 * todoapi.h
 *
 * Created: 2022/4/15 2022 by blueghost
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
#ifndef TODOAPI_H
#define TODOAPI_H

#include <QObject>

#include "../../BGCMS/cmsapi.h"

class TodoApi : public QObject {
    Q_OBJECT
public:
    explicit TodoApi(QObject* parent = nullptr);

    void setCMSApi(CMSApi* api);
    CMSApi* cmsApi() const;

    void list(const QString& lid, CallGraph* pcg, const QString& to,
              const QString& error);
    void newTodo(const QString& lid, CallGraph* pcg, const QString& to,
                 const QString& error);
    void removeAllCompleted(const QString& lid, CallGraph* pcg,
                            const QString& to, const QString& error);
    void remove(int id, CallGraph* pcg, const QString& to,
                const QString& error);
    void content(int id, const QVariant& content, CallGraph* pcg,
                 const QString& to, const QString& error);
    void priority(int id, const QVariant& pri, CallGraph* pcg,
                  const QString& to, const QString& error);
    void state(int id, const QVariant& state, CallGraph* pcg,
               const QString& to, const QString& error);
    void label(int id, const QVariant& varLabel, CallGraph* pcg,
               const QString& to, const QString& error);

signals:
    void todoCreated(const QString& lid, const QVariantMap& todo);
    void contentUpdated(int id, const QString& content);
    void priorityUpdated(int id, int priority);
    void stateUpdated(int id, int state);
    void labelUpdated(int id, const QString& label);
    void removed(int id);
    void allCompletedRemoved(const QString& lid, const QList<int>& ids);

private:
    CMSApi* m_cmsApi;
};

#endif  // TODOAPI_H
