/*****************************************************************************
 * cmswatch.cpp
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
#include "cmswatch.h"

#include <QGuiApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QTimer>

CMSWatch::CMSWatch(QObject* parent) : QObject(parent) {}

pluginUI
CMSWatch::initial(CMSApi* api, CMSBrowserBase* browser, const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    m_logBrowser = new QTextBrowser;
    m_logBrowser->setObjectName("RPCLog");
    m_logBrowser->setWindowTitle(tr("RPC Log"));
    m_logBrowser->setProperty("dockWidgetArea", Qt::BottomDockWidgetArea);

    QTimer* timer = new QTimer(ui.mainWindow);

    QObject::connect(
        m_api, &CMSApi::stateChanged, this,
        [=](QAbstractSocket::SocketState state) {
            switch (state) {
            case QAbstractSocket::UnconnectedState:
                m_logBrowser->append(tr("Disconnected"));
                timer->stop();
                if (m_api->isReconnected() && !m_requestReconnected) {
                    timer->stop();
                    if (QMessageBox::question(ui.mainWindow, "是否重连?",
                                              "网络意外中断,是否重连?") ==
                        QMessageBox::Yes) {
                        m_requestReconnected = true;
                        m_api->connectToHost();
                    }
                }
                break;
            case QAbstractSocket::ConnectingState:
                m_logBrowser->append(tr("Connecting..."));
                break;
            case QAbstractSocket::ConnectedState:
                m_requestReconnected = false;
                m_logBrowser->append(tr("Connected"));
                m_pingpong = 0;
                m_wathcPongCount = 0;
                timer->start(250);
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

            m_callCount++;
        });
    QObject::connect(
        m_api, &CMSApi::returned, this, [=](const QJsonDocument& data) {
            QString mID = data.toVariant().toMap()["mID"].toString();
            m_logBrowser->append(
                tr("<strong style=\"color:#999\">[RETURNED]</strong>[") + mID +
                "]" + m_callHistory.take(mID).toString());

            m_returnCount++;
        });
    QObject::connect(
        m_api, &CMSApi::remoteSignal, this,
        [=](const QString& obj, const QString& sig, const QJsonArray&) {
            m_logBrowser->append(
                tr("<strong style=\"color:#990\">[SIGNAL]</strong>") + obj +
                "." + sig);

            m_signalCount++;
        });
    QObject::connect(
        m_api, &CMSApi::error, this, [=](const QJsonDocument& data) {
            QVariantMap errorData = data.toVariant().toMap();
            m_logBrowser->append(
                tr("<strong style=\"color:#f00\">[ERROR]</strong>") +
                m_callHistory.take(errorData["mID"].toString()).toString() +
                " - " + errorData["error"].toString());
        });

    QObject::connect(m_cmsBrowser, SIGNAL(logMessage(QString)), this,
                     SLOT(browserMessage(QString)));

    QObject::connect(m_api, &CMSApi::ping, this, [=]() {
        m_pingpong = 1;
        rpcProgress();
    });
    QObject::connect(m_api, &CMSApi::pong, this, [=]() {
        m_pingpong = 2;
        m_wathcPongCount = 0;
    });

    QStatusBar* statusBar = ui.mainWindow->statusBar();
    QLabel* label = new QLabel(ui.mainWindow);
    statusBar->insertPermanentWidget(0, label);

    QObject::connect(timer, &QTimer::timeout, this, [=]() {
        label->setText(rpcProgress());
        m_callCount = 0;
        m_returnCount = 0;
        m_signalCount = 0;
        m_wathcPongCount++;
        if (m_wathcPongCount >= 100) {
            /*if (!m_requestReconnected &&
                QMessageBox::question(ui.mainWindow, "是否重连?",
                                      R"(由于长时间未收到服务器的Pong信号,)"
                                      R"(可能已断开连接,是否重连)") ==
                    QMessageBox::Yes) {
                m_api->connectToHost();
                m_requestReconnected = true;
            }*/
            if (!m_requestReconnected) {
                timer->stop();
                if (QMessageBox::question(ui.mainWindow, "是否重连?",
                                          R"(由于长时间未收到服务器的Pong信号,)"
                                          "\n"
                                          R"(可能已断开连接,是否重连)") ==
                    QMessageBox::Yes) {
                    m_requestReconnected = true;
                    m_api->connectToHost();
                } else
                    timer->start();
            }
            m_wathcPongCount = 0;
        }

        if (m_pingpong != 1) m_pingpong = 0;
    });

    QObject::connect(qGuiApp, &QGuiApplication::aboutToQuit, this,
                     [=]() { m_api->disconnectFromHost(); });

    pluginUI _pluginUI;
    _pluginUI.docker = m_logBrowser;

    return _pluginUI;
}

void
CMSWatch::browserMessage(const QString& message) {
    m_logBrowser->append(tr("<strong style=\"color:#900\">[browser]</strong>") +
                         message);
}

QString
CMSWatch::rpcProgress(int count) {
    QString status;
    status.fill(u'⚪', 3);
    for (int i = 0; i < qMin(count, 3); ++i) status[i] = u'⚫';

    return status;
}

QString
CMSWatch::rpcProgress() {
    return rpcProgress(m_callCount) + "  " + rpcProgress(m_returnCount) + "  " +
           rpcProgress(m_signalCount) + "  " +
           (m_pingpong == 0 ? u'◯' : (m_pingpong == 1 ? u'◉' : u'●'));
}
