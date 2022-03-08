/*****************************************************************************
 * rpclog.cpp
 *
 * Created: 2022/2/8 2022 by blueghost
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
#include "rpclog.h"

RPCLog::RPCLog(QObject* parent) : QObject(parent) {}

pluginUI
RPCLog::initial(CMSApi* api, CMSBrowserBase* browser, const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    m_logBrowser = new QTextBrowser;
    m_logBrowser->setObjectName("RPCLog");
    m_logBrowser->setWindowTitle(tr("RPC Log"));
    m_logBrowser->setProperty("dockWidgetArea", Qt::BottomDockWidgetArea);

    QObject::connect(m_api, &CMSApi::stateChanged, this,
                     [=](QAbstractSocket::SocketState state) {
                         switch (state) {
                         case QAbstractSocket::UnconnectedState:
                             m_logBrowser->append(tr("Disconnected"));
                             break;
                         case QAbstractSocket::ConnectingState:
                             m_logBrowser->append(tr("Connecting..."));
                             break;
                         case QAbstractSocket::ConnectedState:
                             m_logBrowser->append(tr("Connected"));
                             break;
                         default:
                             break;
                         }
                     });

    QObject::connect(
        m_api, &CMSApi::calling, this,
        [=](const QString& mID, const QString& obj, const QString& method) {
            QString objMethod = obj + "." + method;
            m_callHistory[mID] = objMethod;
            m_logBrowser->append(
                tr("<strong style=\"color:#090\">[CALLING]</strong>[") + mID +
                "]" + objMethod);
        });
    QObject::connect(
        m_api, &CMSApi::returned, this, [=](const QJsonDocument& data) {
            QString mID = data.toVariant().toMap()["mID"].toString();
            m_logBrowser->append(
                tr("<strong style=\"color:#999\">[RETURNED]</strong>[") + mID +
                "]" + m_callHistory.take(mID).toString());
        });
    QObject::connect(
        m_api, &CMSApi::remoteSignal, this,
        [=](const QString& obj, const QString& sig, const QJsonArray&) {
            m_logBrowser->append(
                tr("<strong style=\"color:#990\">[SIGNAL]</strong>") + obj +
                "." + sig);
        });

    QObject::connect(m_cmsBrowser, SIGNAL(logMessage(QString)), this,
                     SLOT(browserMessage(QString)));

    pluginUI _pluginUI;
    _pluginUI.docker = m_logBrowser;

    return _pluginUI;
}

void
RPCLog::browserMessage(const QString& message) {
    m_logBrowser->append(tr("<strong style=\"color:#900\">[browser]</strong>") +
                         message);
}
