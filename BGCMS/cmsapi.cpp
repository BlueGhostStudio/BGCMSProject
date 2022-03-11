/*****************************************************************************
 * api.cpp
 *
 * Created: 2022/1/18 2022 by BlueGhost Studio
 *
 * Copyright 2022 BlueGhost Studio. All rights reserved.
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

#include "cmsapi.h"

#include <QDebug>

CMSApi::CMSApi(QObject* parent) : NS_BGMRPCClient::BGMRPCClient(parent) {
    QObject::connect(
        this, &CMSApi::remoteSignal, this,
        [=](const QString& obj, const QString& sig, const QJsonArray& args) {
            if (obj == CMSObjID("main")) {
                if (sig == "nodeCreated")
                    emit nodeCreated(
                        args[0].toVariant().toMap()["node"].toMap());
                else if (sig == "nodeRenamed")
                    emit nodeRenamed(args[0].toVariant(), args[1].toString());
                else if (sig == "nodeRemoved")
                    emit nodeRemoved(args[0].toVariant());
                else if (sig == "resourceNodeRemoved")
                    emit resourceNodeRemoved(args[0], args[1].toString(),
                                             args[2].toString());
                else if (sig == "nodeUpdated")
                    emit nodeUpdated(
                        args[0].toInt(),
                        args[1].toVariant().toMap()["node"].toMap());
                else if (sig == "nodeCopied")
                    emit nodeCopied(
                        args[0].toVariant().toMap()["node"].toMap());
                else if (sig == "nodeMoved")
                    emit nodeMoved(args[0].toVariant().toMap()["node"].toMap(),
                                   args[1].toVariant());
            }
        });
}

QString
CMSApi::grp() const {
    return m_grp;
}

void
CMSApi::setGrp(const QString& grp) {
    m_grp = grp;
}

QString
CMSApi::CMSObjID(const QString& obj) {
    QString objID = "cms::" + obj;
    if (!m_grp.isEmpty())
        return m_grp + "::" + objID;
    else
        return objID;
}

void
CMSApi::join(QPointer<CallGraph> pcg, const QString& to) {
    if (!pcg.isNull() && !to.isEmpty())
        callMethod(CMSObjID("main"), "join", {})
            ->then([=](const QVariant& ret) { pcg->to(to, ret.toList()[0]); });
}

void
CMSApi::login(const QString& token, QPointer<CallGraph> pcg, const QString& to,
              const QString& error) {
    if (!pcg.isNull() && !to.isEmpty())
        callMethod("account", "login", { token })
            ->then([=](const QVariant& ret) {
                if (ret.toList()[0].toBool()) {
                    pcg->to(to);
                } else if (!error.isEmpty())
                    pcg->to(error);
            });
}

void
CMSApi::addNode(const QVariant& pNode, const QVariantMap& nodeData,
                QPointer<CallGraph> pcg, const QString& to,
                const QString& error) {
    if (!pcg.isNull() && !to.isEmpty())
        callMethod(CMSObjID("main"), "newNode", { pNode, nodeData })
            ->then([=](const QVariant& ret) {
                QVariantMap d = ret.toList()[0].toMap();
                if (d["ok"].toBool()) {
                    pcg->to(to, d["node"]);
                } else if (!error.isEmpty())
                    pcg->to(error, d["error"]);
            });
}

void
CMSApi::removeNode(const QVariant& node, QPointer<CallGraph> pcg,
                   const QString& to, const QString& error) {
    if (node.toInt() <= 0) {
        if (!pcg.isNull() && !error.isEmpty())
            pcg->to(error, "can't remove \'..\' node");
    } else if (!pcg.isNull() && !to.isEmpty())  //{
        callMethod(CMSObjID("main"), "removeNode", { node })
            ->then([=](const QVariant& ret) {
                QVariantMap d = ret.toList()[0].toMap();
                if (d["ok"].toBool()) {
                    if (!pcg.isNull()) pcg->to(to, node);
                } else if (!pcg.isNull() && !error.isEmpty())
                    pcg->to(error, d["error"]);
            });
}

void
CMSApi::node(const QVariant& node, QPointer<CallGraph> pcg, const QString& to,
             const QString& error) {
    if (!pcg.isNull() && !to.isEmpty())
        callMethod(CMSObjID("main"), "node", { node })
            ->then([=](const QVariant& ret) {
                QVariantMap d = ret.toList()[0].toMap();
                if (d["ok"].toBool()) {
                    if (!pcg.isNull()) pcg->to(to, d["node"]);
                } else if (!pcg.isNull() && !error.isEmpty())
                    pcg->to(error, d["error"]);
            });
}

void
CMSApi::nodeInfo(const QVariant& node, QPointer<CallGraph> pcg,
                 const QString& to, const QString& error) {
    if (!pcg.isNull() && !to.isEmpty())
        callMethod(CMSObjID("main"), "nodeInfo", { node })
            ->then([=](const QVariant& ret) {
                QVariantList retList = ret.toList();
                QVariantMap ret0 = retList[0].toMap();

                if (ret0["ok"].toBool()) {
                    if (!pcg.isNull()) pcg->to(to, ret0["node"]);
                } else if (!pcg.isNull() && !error.isEmpty())
                    pcg->to(error, ret0["error"]);
            });
}

void
CMSApi::nodePath(const QVariant& node, QPointer<CallGraph> pcg,
                 const QString& to) {
    if (!pcg.isNull() && !to.isEmpty())
        callMethod(CMSObjID("main"), "nodePath", { node })
            ->then([=](const QVariant& ret) {
                if (!pcg.isNull()) pcg->to(to, ret.toList()[0]);
            });
}

void
CMSApi::loadNodes(const QVariant& pNode, QPointer<CallGraph> pcg,
                  const QString& to, const QString& error) {
    if (!pcg.isNull() && !to.isEmpty())
        callMethod(CMSObjID("main"), "list", { pNode })
            ->then([=](const QVariant& ret) {
                QVariant d = ret.toList()[0];
                QVariantMap m = d.toMap();
                if (m["ok"].toBool()) {
                    if (!pcg.isNull()) pcg->to(to, d);
                } else if (!pcg.isNull() && !error.isEmpty())
                    pcg->to(error, d);
            });
}

void
CMSApi::updateNode(const QVariant& node, const QVariantMap& newData,
                   QPointer<CallGraph> pcg, const QString& to,
                   const QString& error) {
    if (pcg && !to.isEmpty())
        callMethod(CMSObjID("main"), "updateNode", { node, newData })
            ->then([=](const QVariant& ret) {
                QVariantMap d = ret.toList()[0].toMap();
                if (d["ok"].toBool()) {
                    if (!pcg.isNull()) pcg->to(to, d["node"]);
                } else if (!pcg.isNull() && !error.isEmpty())
                    pcg->to(error, d["error"]);
            });
}

void
CMSApi::moveNode(const QVariant& source, const QVariant& target,
                 QPointer<CallGraph> pcg, const QString& to,
                 const QString& error) {
    if (source.toInt() <= 0) {
        if (!pcg.isNull() && !error.isEmpty())
            pcg->to(error, "can't move \'..\' node");
    } else if (!pcg.isNull() && !to.isEmpty())
        callMethod(CMSObjID("main"), "moveNode", { source, target })
            ->then([=](const QVariant& ret) {
                QVariantMap d = ret.toList()[0].toMap();
                if (d["ok"].toBool()) {
                    if (!pcg.isNull()) pcg->to(to, d["node"]);
                } else if (!pcg.isNull() && !error.isEmpty())
                    pcg->to(error, d["error"]);
            });
}

void
CMSApi::copyNode(const QVariant& source, const QVariant& target,
                 QPointer<CallGraph> pcg, const QString& to,
                 const QString& error) {
    if (source.toInt() <= 0) {
        if (!pcg.isNull() && !error.isEmpty())
            pcg->to(error, "can't copy \'..\' node");
    } else if (pcg && !to.isEmpty())
        callMethod(CMSObjID("main"), "copyNode", { source, target })
            ->then([=](const QVariant& ret) {
                QVariantMap d = ret.toList()[0].toMap();
                if (d["ok"].toBool()) {
                    if (!pcg.isNull()) pcg->to(to, d["node"]);
                } else if (!pcg.isNull() && !error.isEmpty())
                    pcg->to(error, d["error"]);
            });
}
