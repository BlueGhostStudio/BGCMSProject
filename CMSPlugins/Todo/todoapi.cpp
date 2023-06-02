/*****************************************************************************
 * todoapi.cpp
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
#include "todoapi.h"

TodoApi::TodoApi(QObject* parent) : QObject{ parent } {}

void
TodoApi::setCMSApi(CMSApi* api) {
    m_cmsApi = api;

    QObject::connect(
        api, &CMSApi::remoteSignal, this,
        [=](const QString& obj, const QString& sig, const QJsonArray& args) {
            qDebug() << obj << sig << args << m_cmsApi->CMSObjID("todo::main");
            if (obj == m_cmsApi->CMSObjID("todo::main")) {
                if (sig == "newRecord") {
                    QVariantMap todo = args[0].toVariant().toMap();
                    emit todoCreated(todo["lid"].toString(), todo);
                } else if (sig == "contentUpdated")
                    emit contentUpdated(args[0].toInt(), args[1].toString());
                else if (sig == "priorityUpdated")
                    emit priorityUpdated(args[0].toInt(), args[1].toInt());
                else if (sig == "stateUpdated")
                    emit stateUpdated(args[0].toInt(), args[1].toInt());
                else if (sig == "labelUpdated")
                    emit labelUpdated(args[0].toInt(), args[1].toString());
                else if (sig == "removed")
                    emit removed(args[0].toInt());
                else if (sig == "allCompletedRemoved") {
                    qDebug() << args[1].toVariant().value<QList<int>>();
                    QList<int> ids;
                    foreach (const QVariant& id, args[1].toVariant().toList())
                        ids.append(id.toInt());

                    emit allCompletedRemoved(args[0].toString(), ids);
                }
            }
        });
    QObject::connect(m_cmsApi, &CMSApi::connected, this, [=]() {
        m_cmsApi->callMethod(m_cmsApi->CMSObjID("todo::main"), "join", {})
            ->then([=](const QVariant& ret) { qDebug() << ret; });
    });
}

CMSApi*
TodoApi::cmsApi() const {
    return m_cmsApi;
}

void
TodoApi::list(const QString& lid, CallGraph* pcg, const QString& to,
              const QString& error) {
    m_cmsApi->callMethod(m_cmsApi->CMSObjID("todo::main"), "list", { lid })
        ->then([=](const QVariant& ret) {
            QVariantList retList = ret.toList();
            QVariant ret0 = retList[0];

            if (ret0.typeId() == QMetaType::QVariantList && pcg/*.isNull()*/)
                pcg->to(to, ret0);
            else if (!ret0.toBool() && pcg/*.isNull()*/ && !error.isEmpty())
                pcg->to(error);
        });
}

void
TodoApi::newTodo(const QString& lid, CallGraph* pcg, const QString& to,
                 const QString& error) {
    m_cmsApi
        ->callMethod(m_cmsApi->CMSObjID("todo::main"), "newRecord",
                     { lid, ">> New todo <<" })
        ->then([=](const QVariant& ret) {
            QVariantList retList = ret.toList();
            QVariant ret0 = retList[0];

            if (ret0.typeId() == QMetaType::QVariantMap && pcg/*.isNull()*/)
                pcg->to(to, ret0);
            else if (!ret0.toBool() && pcg/*.isNull()*/ && !error.isEmpty())
                pcg->to(error);
        });
}

void
TodoApi::removeAllCompleted(const QString& lid, CallGraph* pcg,
                            const QString& to, const QString& error) {
    m_cmsApi
        ->callMethod(m_cmsApi->CMSObjID("todo::main"), "removeAllCompleted",
                     { lid })
        ->then([=](const QVariant& ret) {
            QVariantList retList = ret.toList();
            QVariant ret0 = retList[0];

            if (ret0.toBool() && pcg/*.isNull()*/)
                pcg->to(to);
            else if (pcg/*.isNull()*/ && !error.isEmpty())
                pcg->to(error);
        });
}

void
TodoApi::remove(int id, CallGraph* pcg, const QString& to,
                const QString& error) {
    m_cmsApi->callMethod(m_cmsApi->CMSObjID("todo::main"), "remove", { id })
        ->then([=](const QVariant& ret) {
            QVariantList retList = ret.toList();
            QVariant ret0 = retList[0];

            if (ret0.toBool() && pcg/*.isNull()*/)
                pcg->to(to);
            else if (pcg/*.isNull()*/ && !error.isEmpty())
                pcg->to(error);
        });
}

void
TodoApi::content(int id, const QVariant& content, CallGraph* pcg,
                 const QString& to, const QString& error) {
    m_cmsApi
        ->callMethod(m_cmsApi->CMSObjID("todo::main"), "content",
                     { id, content })
        ->then([=](const QVariant& ret) {
            QVariantList retList = ret.toList();
            QVariant ret0 = retList[0];

            if (ret0.typeId() == QMetaType::QString && pcg/*.isNull()*/)
                pcg->to(to, ret0);
            else if (!ret0.toBool() && pcg/*.isNull()*/ && !error.isEmpty())
                pcg->to(error);
        });
}

void
TodoApi::priority(int id, const QVariant& pri, CallGraph* pcg,
                  const QString& to, const QString& error) {
    m_cmsApi
        ->callMethod(m_cmsApi->CMSObjID("todo::main"), "priority", { id, pri })
        ->then([=](const QVariant& ret) {
            QVariantList retList = ret.toList();
            QVariant ret0 = retList[0];

            if (ret0.typeId() == QMetaType::Int && pcg/*.isNull()*/)
                pcg->to(to, ret0);
            else if (!ret0.toBool() && pcg/*.isNull()*/ && !error.isEmpty())
                pcg->to(error);
        });
}

void
TodoApi::state(int id, const QVariant& state, CallGraph* pcg,
               const QString& to, const QString& error) {
    m_cmsApi
        ->callMethod(m_cmsApi->CMSObjID("todo::main"), "state", { id, state })
        ->then([=](const QVariant& ret) {
            QVariantList retList = ret.toList();
            QVariant ret0 = retList[0];

            qDebug() << ret0.typeId();

            if (ret0.typeId() == QMetaType::LongLong && pcg/*.isNull()*/)
                pcg->to(to, ret0);
            else if (!ret0.toBool() && pcg/*.isNull()*/ && !error.isEmpty())
                pcg->to(error);
        });
}

void
TodoApi::label(int id, const QVariant& label, CallGraph* pcg,
               const QString& to, const QString& error) {
    m_cmsApi
        ->callMethod(m_cmsApi->CMSObjID("todo::main"), "label", { id, label })
        ->then([=](const QVariant& ret) {
            QVariantList retList = ret.toList();
            QVariant ret0 = retList[0];

            if (ret0.typeId() == QMetaType::QString && pcg/*.isNull()*/)
                pcg->to(to, ret0);
            else if (!ret0.toBool() && pcg/*.isNull()*/ && !error.isEmpty())
                pcg->to(error);
        });
}
