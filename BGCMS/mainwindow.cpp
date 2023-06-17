#include "mainwindow.h"

#include <QActionGroup>
#include <QClipboard>
#include <QDebug>
#include <QDockWidget>
#include <QInputDialog>
#include <QMetaType>
#include <QPluginLoader>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include <QToolButton>

#include "../CMSPlugins/cmsplugininterface.h"
#include "hosturldialog.h"
#include "newnodedialog.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi(this);
    initial();
}

MainWindow::~MainWindow() {}

void
MainWindow::closeEvent(QCloseEvent* /*ev*/) {
    QSettings settings;
    settings.setValue("winState", saveState());
    settings.setValue("geometry", saveGeometry());
    qApp->closeAllWindows();
}

void
MainWindow::setCMSGrp() {
    bool ok;
    QString grp =
        QInputDialog::getText(this, tr("Instance of CMS"), tr("Instance Name"),
                              QLineEdit::Normal, m_api.grp(), &ok);
    if (ok) {
        m_api.setGrp(grp);
        QSettings settings;
        settings.setValue("remote/group", grp);
        m_api.disconnectFromHost();
    }
}

void
MainWindow::connect2Host() {
    // m_api.connectToHost(QUrl("wss://rpc.bgstudio.live:8000"));
    // m_api.connectToHost(QUrl("ws://72.167.55.234:8000"));
    // m_api.connectToHost(QUrl("wss://rpc.bgstudio.live:8000"));
    if (!m_api.isReconnected()) {
        HostUrlDialog* urlDialog = new HostUrlDialog(this);
        urlDialog->setUrl(QSettings().value("remote/host").toString());
        QObject::connect(
            urlDialog, &HostUrlDialog::finished, this, [=](int result) {
                if (result == 1) {
                    m_api.connectToHost(urlDialog->url());
                    QSettings().setValue("remote/host", urlDialog->urlString());
                }
            });
        urlDialog->open();
    } else
        m_api.connectToHost();
}

void
MainWindow::disconnect() {
    m_api.disconnectFromHost();
}

void
MainWindow::uiWorkStatus(bool working) {
    uiStatus(!working);
}

void
MainWindow::statusMessage(const QString& message, int timeOut) {
    ui_statusbar->showMessage(message, timeOut);
}

void
MainWindow::changeAccount() {
    CallGraph::start("changeAccount", this)
        ->nodes("changeAccount",
                [=](CallGraph* cg, const QVariant&) {
                    m_api.login(token(true), cg, "home", "loginErr");
                })
        ->nodes("loginErr",
                [=](CallGraph* cg, const QVariant&) {
                    qDebug() << "Login fail";
                    statusMessage(tr("Login fail"));
                    cg->to("home");
                })
        ->nodes("home",
                [=](CallGraph* cg, const QVariant&) {
                    ui_cmsBrowser->load("/", cg, "end", "end");
                })
        ->nodes("end", [=](CallGraph* cg, const QVariant&) { cg->toFinal(); })
        ->exec();
}

void
MainWindow::copyLinkFormatChanged() {
    QSettings().setValue("copyLinkFormat",
                         m_actgrpCopyLinkFormat->checkedAction()->data());
}

void
MainWindow::newBlankNode() {
    CallGraph::start("openDlg", this)
        ->nodes("openDlg",
                [=](CallGraph* cg, const QVariant&) {
                    NewNodeDialog* dlg =
                        new NewNodeDialog(cg, "accept", "", this);
                    dlg->open();
                })
        ->nodes("accept",
                [=](CallGraph* cg, const QVariant& data) {
                    m_api.addNode(ui_cmsBrowser->listNode(), data.toMap(), cg,
                                  "end", "error");
                })
        ->nodes("error",
                [=](CallGraph* cg, const QVariant& data) {
                    statusMessage(tr("ERROR: ") + data.toString());

                    cg->to("end");
                })
        ->nodes("end", [=](CallGraph* cg, const QVariant&) { cg->toFinal(); })
        ->exec();
}

void
MainWindow::initial() {
    QSettings settings;

    m_api.setGrp(settings.value("remote/group").toString());
    QToolButton* tbtnNewNode = new QToolButton;
    tbtnNewNode->setDefaultAction(action_new);

    m_newNodeMenu = new QMenu(tr("New node"));

    tbtnNewNode->setMenu(m_newNodeMenu);
    tbtnNewNode->setPopupMode(QToolButton::MenuButtonPopup);
    ui_tb_main->insertWidget(action_remove, tbtnNewNode);

    uiConnectStatus(false);

    ui_cmsBrowser->initial(&m_api);
    action_remove->setEnabled(ui_cmsBrowser->hasSelection());

    QObject::connect(
        &m_api, &CMSApi::isConnectedChanged, this,
        [=](bool status, bool reconnected) {
            if (status) {
                CallGraph::start("afterConnected", this)
                    ->nodes("afterConnected",
                            [=](CallGraph* cg, const QVariant&) {
                                m_api.join(cg, "afterJoin");
                            })
                    ->nodes("afterJoin",
                            [=](CallGraph* cg, const QVariant& data) {
                                m_api.setProperty("cID", data);
                                cg->to("login");
                            })
                    ->nodes("login",
                            [=](CallGraph* cg, const QVariant&) {
                                QString to = reconnected ? "updateUi" : "home";
                                m_api.login(token(), cg, to, "loginErr");
                            })
                    ->nodes("loginErr",
                            [=](CallGraph* cg, const QVariant&) {
                                statusMessage(tr("Login fail"));
                                if (!reconnected)
                                    cg->to("home");
                                else
                                    cg->to("updateUi");
                            })
                    ->nodes("home",
                            [=](CallGraph* cg, const QVariant&) {
                                ui_cmsBrowser->load("/", cg, "updateUi",
                                                    "updateUi");
                            })
                    ->nodes("updateUi",
                            [=](CallGraph* cg, const QVariant&) {
                                uiConnectStatus(true);
                                cg->toFinal();
                            })
                    ->exec();
            } else
                uiConnectStatus(false);
        });
    QObject::connect(&m_api, &CMSApi::stateChanged, this,
                     [=](QAbstractSocket::SocketState state) {
                         switch (state) {
                         case QAbstractSocket::UnconnectedState:
                             statusMessage(tr("Disconnected"));
                             m_connectStatus->setPixmap(m_disconnectedPixmap);
                             break;
                         case QAbstractSocket::ConnectingState:
                             statusMessage(tr("Connecting..."), 0);
                             break;
                         case QAbstractSocket::ConnectedState:
                             statusMessage(tr("Connected"));
                             m_connectStatus->setPixmap(m_connectedPixmap);
                             break;
                         default:
                             break;
                         }
                     });

    QLineEdit* pathEdit = new QLineEdit;
    pathEdit->setFocusPolicy(Qt::ClickFocus);
    ui_tb_path->addWidget(pathEdit);
    insertToolBarBreak(ui_tb_path);

    QObject::connect(
        ui_cmsBrowser, &CMSBrowser::selectedNodeChanged, this,
        [=](const QItemSelection& selected, const QItemSelection& deselected) {
            action_remove->setEnabled(ui_cmsBrowser->hasSelection());
            action_copy->setEnabled(ui_cmsBrowser->hasSelection());
            action_cut->setEnabled(ui_cmsBrowser->hasSelection());
        });
    QObject::connect(ui_cmsBrowser, &CMSBrowser::logMessage, this,
                     &MainWindow::statusMessage);
    QObject::connect(ui_cmsBrowser, &CMSBrowser::working, this,
                     &MainWindow::uiWorkStatus);

    QObject::connect(ui_cmsBrowser, &CMSBrowser::pathChanged, pathEdit,
                     &QLineEdit::setText);
    QObject::connect(pathEdit, &QLineEdit::returnPressed, this,
                     [=]() { ui_cmsBrowser->load(pathEdit->text()); });

    QObject::connect(
        &m_api, &CMSApi::calling, this,
        [=](const QString& mID, const QString& obj, const QString& method) {
            QString objMethod = obj + "." + method;
            statusMessage(tr("Calling ") + objMethod);
            m_callHistory[mID] = objMethod;
        });
    QObject::connect(
        &m_api, &CMSApi::returned, this, [=](const QJsonDocument& data) {
            QString mID = data.toVariant().toMap()["mID"].toString();
            statusMessage(m_callHistory.take(mID).toString() + " returned");
        });
    QObject::connect(
        &m_api, &CMSApi::remoteSignal, this,
        [=](const QString& obj, const QString& sig, const QJsonArray&) {
            statusMessage(tr("Signal ") + obj + "." + sig);
        });

    QClipboard* clipboard = QGuiApplication::clipboard();
    action_copy->setEnabled(false);
    action_cut->setEnabled(false);
    action_paste->setEnabled(hasClip());
    action_paste_reference->setEnabled(hasClip(true));
    QObject::connect(clipboard, &QClipboard::dataChanged, this, [=]() {
        action_paste->setEnabled(hasClip());
        action_paste_reference->setEnabled(hasClip(true));
    });

    m_actgrpCopyLinkFormat = new QActionGroup(this);
    action_copy_link_href->setData(0);
    m_actgrpCopyLinkFormat->addAction(action_copy_link_href);
    action_copy_link_html->setData(1);
    m_actgrpCopyLinkFormat->addAction(action_copy_link_html);
    action_copy_link_md->setData(2);
    m_actgrpCopyLinkFormat->addAction(action_copy_link_md);
    action_copy_link_id->setData(3);
    m_actgrpCopyLinkFormat->addAction(action_copy_link_id);

    switch (settings.value("copyLinkFormat", 0).toInt()) {
    case 0:
        action_copy_link_href->setChecked(true);
        break;
    case 1:
        action_copy_link_html->setChecked(true);
        break;
    case 2:
        action_copy_link_md->setChecked(true);
        break;
    case 3:
        action_copy_link_id->setChecked(true);
    }

    m_connectedPixmap.load(":/imgs/connected.png");
    m_disconnectedPixmap.load(":/imgs/disconnected.png");
    m_connectStatus = new QLabel(this);
    m_connectStatus->setPixmap(m_disconnectedPixmap);
    ui_statusbar->addPermanentWidget(m_connectStatus);

    initialPlugins();

    restoreState(settings.value("winState").toByteArray());
    restoreGeometry(settings.value("geometry").toByteArray());
}

void
MainWindow::initialPlugins() {
    auto loadPlugins = [=](const QString& location) {
        qDebug() << location + "/plugins";
        QDir pluginsDir(location + "/plugins");

        for (QFileInfo& pluginFile : pluginsDir.entryInfoList(QDir::Files)) {
            qDebug() << pluginFile.absoluteFilePath();
            QPluginLoader loader(pluginFile.absoluteFilePath());
            loader.setLoadHints(QLibrary::ResolveAllSymbolsHint);
            QObject* instance = loader.instance();
            CMSPluginInterface* obj =
                qobject_cast<CMSPluginInterface*>(instance);
            if (obj) {
                qDebug() << pluginFile.fileName() << "load ok";

                pluginUI _pluginUI;
                _pluginUI.newNodeMenu = m_newNodeMenu;
                _pluginUI.pluginsMenu = ui_menu_plugins;
                _pluginUI.settingMenu = ui_menu_settings;
                _pluginUI.pluginsToolbar = ui_tb_plugins;
                _pluginUI.mainWindow = this;
                _pluginUI = obj->initial(&m_api, ui_cmsBrowser, _pluginUI);
                QWidget* pluginDocker = _pluginUI.docker;
                QObject::connect(instance, SIGNAL(logMessage(QString, int)),
                                 this, SLOT(statusMessage(QString, int)));
                QObject::connect(instance, SIGNAL(working(bool)), this,
                                 SLOT(uiWorkStatus(bool)));
                if (pluginDocker) {
                    QDockWidget* pluginDock = new QDockWidget;
                    bool ok = false;
                    int area =
                        pluginDocker->property("dockWidgetArea").toInt(&ok);
                    pluginDock->setObjectName(pluginDocker->objectName());
                    pluginDock->setWindowTitle(pluginDocker->windowTitle());
                    pluginDock->setWidget(pluginDocker);
                    addDockWidget(
                        ok ? (Qt::DockWidgetArea)area : Qt::RightDockWidgetArea,
                        pluginDock);

                    pluginDock->setEnabled(false);
                    QObject::connect(&m_api, &CMSApi::isConnectedChanged,
                                     pluginDock, &QDockWidget::setEnabled);
                }
            } else
                statusMessage(tr("Load plugin fail"));
        }
    };

    QStringList dataLocations =
        QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);

#ifdef CMSDATALOCATION
    if (!dataLocations.contains(CMSDATALOCATION))
        dataLocations.append(CMSDATALOCATION);

    qDebug() << CMSDATALOCATION;
#endif

    foreach (const QString& location, dataLocations) loadPlugins(location);
}

void
MainWindow::uiConnectStatus(bool connected) {
    action_connectToHost->setEnabled(!connected);
    action_disconnect->setEnabled(connected);
    action_changeAccount->setEnabled(connected);

    uiStatus(connected);
}

void
MainWindow::uiStatus(bool enabled) {
    centralwidget->setEnabled(enabled);
    ui_tb_main->setEnabled(enabled);
    ui_tb_path->setEnabled(enabled);
    ui_tb_plugins->setEnabled(enabled);
    ui_menu_plugins->setEnabled(enabled);
}

bool
MainWindow::hasClip(bool ref) const {
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    if (ref)
        return mimeData->hasFormat(__MIME_COPY__);
    else
        return mimeData->hasFormat(__MIME_COPY__) ||
               mimeData->hasFormat(__MIME_CUT__) ||
               mimeData->hasFormat(__MIME_PLUGIN__);
}

QString
MainWindow::token(bool ch) {
    QSettings settings;
    if (settings.contains("token") && !ch)
        return settings.value("token").toString();
    else {
        QString token = QInputDialog::getText(this, tr("Login"), tr("token"));
        if (!token.isEmpty()) settings.setValue("token", token);

        return token;
    }
}
