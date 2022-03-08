/*****************************************************************************
 * api.h
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
#ifndef API_H
#define API_H

#include <bgmrpcclient.h>
#include <callgraph.h>

// extern NS_BGMRPCClient::BGMRPCClient BGCMSClient;

class CMSApi : public NS_BGMRPCClient::BGMRPCClient {
    Q_OBJECT

public:
    CMSApi(QObject* parent = nullptr);

    void join(QPointer<CallGraph> pcg, const QString& to);
    void login(const QString& token, QPointer<CallGraph> pcg, const QString& to,
               const QString& error);
    void addNode(const QVariant& pNode, const QVariantMap& nodeData,
                 QPointer<CallGraph> pcg, const QString& to,
                 const QString& error);
    void removeNode(const QVariant& node, QPointer<CallGraph> pcg,
                    const QString& to, const QString& error);
    void node(const QVariant& node, QPointer<CallGraph> pcg, const QString& to,
              const QString& error);
    void nodeInfo(const QVariant& node, QPointer<CallGraph> pcg,
                  const QString& to, const QString& error);
    void nodePath(const QVariant& node, QPointer<CallGraph> pcg,
                  const QString& to);
    void loadNodes(const QVariant& pNode, QPointer<CallGraph> pcg,
                   const QString& to, const QString& error);
    void updateNode(const QVariant& node, const QVariantMap& newData,
                    QPointer<CallGraph> pcg, const QString& to,
                    const QString& error);
    void moveNode(const QVariant& source, const QVariant& target,
                  QPointer<CallGraph> pcg, const QString& to,
                  const QString& error);
    void copyNode(const QVariant& source, const QVariant& target,
                  QPointer<CallGraph> pcg, const QString& to,
                  const QString& error);
signals:
    void nodeCreated(const QVariantMap& newNode);
    void nodeRenamed(const QVariant& node, const QString& name);
    void nodeRemoved(const QVariant& node);
    void resourceNodeRemoved(const QVariant& node, const QString& contentType,
                             const QString& content);
    void nodeUpdated(int id, const QVariantMap& node);
    void nodeCopied(const QVariantMap& node);
    void nodeMoved(const QVariantMap& node, const QVariant& targetNode);

    /*private:
        NS_BGMRPCClient::BGMRPCClient m_client;*/
};

/*void join(NS_BGMRPCClient::BGMRPCClient* client, CallGraph* pcg,
          const QString& to);
void login(NS_BGMRPCClient::BGMRPCClient* client, const QString& token,
           CallGraph* pcg, const QString& to, const QString& error);
void addNode(NS_BGMRPCClient::BGMRPCClient* client, const QVariant& pNode,
             const QVariantMap& nodeData, CallGraph* pcg, const QString& to,
             const QString& error);
void removeNode(NS_BGMRPCClient::BGMRPCClient* client, const QVariant& node,
                CallGraph* pcg, const QString& to, const QString& error);
void node(NS_BGMRPCClient::BGMRPCClient* client, const QVariant& node,
          CallGraph* pcg, const QString& to, const QString& error);
void loadNodes(NS_BGMRPCClient::BGMRPCClient* client, const QVariant& pNode,
               CallGraph* pcg, const QString& to, const QString& error);
void updateNode(NS_BGMRPCClient::BGMRPCClient* client, const QVariant& node,
                const QVariantMap& newData, CallGraph* pcg, const QString& to,
                const QString& error);
void moveNode(NS_BGMRPCClient::BGMRPCClient* client, const QVariant& source,
              const QVariant& target, CallGraph* pcg, const QString& to,
              const QString& error);
void copyNode(NS_BGMRPCClient::BGMRPCClient* client, const QVariant& source,
              const QVariant& target, CallGraph* pcg, const QString& to,
              const QString& error);*/

#endif  // API_H
