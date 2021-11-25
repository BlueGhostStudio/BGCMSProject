#include "bgcmsmainwin.h"

#include <QMdiSubWindow>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPluginLoader>
#include <QStandardPaths>
#include <QtDebug>

#include "cmsplugininterface.h"
#include "pluginsubwin.h"
//#include "CMSPlugins/pluginsubwinBase.h"
#include "global.h"
#include "imgviewer.h"
#include "logindialog.h"
#include "resourceeditor.h"
#include "ressubwin.h"

BGCMSMainWin::BGCMSMainWin(QWidget* parent)
    : QMainWindow(parent), m_viewMode(this) {
    setupUi(this);

    /*tabifyDockWidget(ui_dwSubWinList, ui_dwPlugins);
    ui_dwSubWinList->raise();*/

    m_viewMode.addAction(actionWindowView);
    m_viewMode.addAction(actionTabedView);
    QObject::connect(&m_viewMode, &QActionGroup::triggered, this,
                     &BGCMSMainWin::changeViewMode);
    changeViewMode(m_viewMode.checkedAction());

    mdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation, true);
    mdiArea->addSubWindow(&ui_cmsSubWin);
    ui_cmsSubWin.setWindowState(Qt::WindowMaximized);

    CMSWidget* cmsWidget = qobject_cast<CMSWidget*>(ui_cmsSubWin.widget());
    QObject::connect(&bgmrpcClient, &NS_BGMRPCClient::BGMRPCClient::connected,
                     this, &BGCMSMainWin::join);
    QObject::connect(cmsWidget, &CMSWidget::openResource, this,
                     &BGCMSMainWin::openResource);
    QObject::connect(cmsWidget, &CMSWidget::closeAllSubWin, this,
                     &BGCMSMainWin::closeAllSubWin);

    initialPlugins();
}

BGCMSMainWin::~BGCMSMainWin() {}

void
BGCMSMainWin::join() {
    CallGraph::start("join")
        ->nodes(
            "join",
            [=](CallGraph* cg, const QVariant&) {
                bgmrpcClient.callMethod("CMS", "join", {})
                    ->then([=](const QVariant&) { cg->to("openLoginDlg"); });
            })
        ->nodes("openLoginDlg",
                [=](CallGraph* cg, const QVariant&) {
                    LoginDialog* loginDialog = new LoginDialog(this);
                    loginDialog->setAttribute(Qt::WA_DeleteOnClose);
                    loginDialog->setToken(m_token);

                    QObject::connect(loginDialog, &LoginDialog::finished, this,
                                     [=](int result) {
                                         if (result == 1)
                                             cg->to("login",
                                                    loginDialog->token());
                                         else if (result == 2)
                                             cg->to("register");
                                         else
                                             cg->to("afterJoined");
                                     });

                    loginDialog->open();
                })
        ->nodes("register",
                [=](CallGraph* cg, const QVariant&) {
                    bgmrpcClient.callMethod("account", "register", {})
                        ->then([=](const QVariant& ret) {
                            QString token = ret.toList()[0].toString();
                            QMessageBox::information(this, tr("注册"),
                                                     tr("令牌:") + token);
                            cg->to("login", token);
                        });
                })
        ->nodes("login",
                [=](CallGraph* cg, const QVariant& data) {
                    m_token = data.toString();
                    bgmrpcClient.callMethod("account", "login", { data })
                        ->then([=](const QVariant& ret) {
                            if (ret.toList()[0].toBool()) {
                                qDebug() << "---- token ---- in mainwin"
                                         << data.toString();
                                emit logined(data.toString());
                                cg->to("afterJoined");
                            } else
                                cg->to("openLoginDlg");
                        });
                })
        ->nodes("afterJoined",
                [=](CallGraph* cg, const QVariant&) {
                    ui_cmsSubWin.cmsWidget()->afterJoined(cg, "end");
                })
        ->nodes("end", [=](CallGraph* cg, const QVariant&) { cg->toFinal(); })
        ->exec();
}

/*void
BGCMSMainWin::login(const QString& token) {
    CallGraph::start("login")
        ->nodes("login",
                [=](CallGraph* cg, const QVariant&) {
                    bgmrpcClient.callMethod("account", "login", { token })
                        ->then([=](const QVariant& ret) {
                            qDebug() << "=====" << ret;
                            cg->toFinal();
                        });
                })
        ->exec();
}*/

void
BGCMSMainWin::changeViewMode(const QAction* action) {
    //    qDebug() << action->property("value").toInt();
    int value = action->property("value").toInt();
    mdiArea->setViewMode((QMdiArea::ViewMode)value);
    if (value == 0)
        ui_dwSubWinList->show();
    else
        ui_dwSubWinList->close();
}

void
BGCMSMainWin::connectToCMS() {
    bgmrpcClient.connectToHost(QUrl("ws://122.114.67.199:8000"));
}

void
BGCMSMainWin::disconnectFromCMS() {
    bgmrpcClient.disconnectFromHost();
}

void
BGCMSMainWin::openResource(const QVariantMap& resource) {
    QString type = resource["type"].toString();
    int rID = resource["id"].toInt();

    SubWinBase* subWin = findSubWin(rID);
    if (!subWin) {
        if (type == "A" || type == "C" || type == "S") {
            ResourceEditor* editor = new ResourceEditor;
            subWin = new ResSubWin(editor, this);
            mdiArea->addSubWindow(subWin);
            subWin->show();
            editor->initialEditor(this, m_editorPlugins);
            editor->openResource(resource);
            QObject::connect(
                editor, &ResourceEditor::hasModified, ui_lvSubWinList,
                QOverload<int, bool>::of(&SubWinList::updateResItem));
        } else {
            ImgViewer* imgViewer = new ImgViewer;
            subWin = new ResSubWin(imgViewer, this);
            mdiArea->addSubWindow(subWin);
            subWin->show();
            imgViewer->openResource(resource);
        }

        emit editorOpened(resource);
        QObject::connect(subWin, &ResSubWin::destroyed, this,
                         [=]() { emit subWinClosed(rID); });
    } else
        mdiArea->setActiveSubWindow(subWin);
}

void
BGCMSMainWin::openPluginWin(QWidget* widget, bool deleteOnClose) {
    SubWinBase* subWin = nullptr;
    bool ok;
    int pID = widget->property("pID").toInt(&ok);
    if (ok) subWin = findSubWin(pID);

    if (!subWin) {
        PluginSubWin* pluginSubWin = new PluginSubWin(widget, this);
        pluginSubWin->setDeleteWidgetOnClose(deleteOnClose);

        subWin = pluginSubWin;

        int winID = subWin->winID();
        widget->setProperty("pID", winID);

        mdiArea->addSubWindow(subWin);
        subWin->show();

        emit pluginSubWinOpened(subWin->winID(), subWin->windowTitle());
        QObject::connect(subWin, &ResSubWin::destroyed, this,
                         [=]() { emit subWinClosed(winID); });
        QObject::connect(subWin, &SubWinBase::windowTitleChanged, this,
                         [=](const QString& title) {
                             ui_lvSubWinList->updatePluginItem(winID, title);
                         });
        QObject::connect(subWin, &SubWinBase::windowIconChanged, this,
                         [=](const QIcon& icon) {
                             ui_lvSubWinList->updatePluginItemIcon(winID, icon);
                         });
    } else
        mdiArea->setActiveSubWindow(subWin);
}

void
BGCMSMainWin::activeSubWin(int id) {
    qDebug() << "activeSubWin====>" << id;
    /*bool opened = false;

    foreach (QMdiSubWindow* subWin, mdiArea->subWindowList()) {
        ResourceEditor* editor =
            qobject_cast<ResourceEditor*>(subWin->widget());
        if (editor && editor->resID() == resID) {
            opened = true;
            mdiArea->setActiveSubWindow(subWin);
        }
    }

    return opened;*/
    QMdiSubWindow* subWin = findSubWin(id);
    if (subWin) mdiArea->setActiveSubWindow(subWin);
}

void
BGCMSMainWin::closeSubWin(int id) {
    QMdiSubWindow* subWin = findSubWin(id);
    if (subWin) subWin->close();
}

void
BGCMSMainWin::closeAllSubWin() {
    mdiArea->closeAllSubWindows();
}

void
BGCMSMainWin::initialPlugins() {
    QDir pluginsDir(QStandardPaths::standardLocations(
                        QStandardPaths::AppLocalDataLocation)[0] +
                    "/plugins");
    for (QString& fileName : pluginsDir.entryList(QDir::Files)) {
        qDebug() << "----" << fileName;
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject* instance = loader.instance();
        m_editorPlugins.append(instance);

        CMSPluginInterface* obj = qobject_cast<CMSPluginInterface*>(instance);
        qDebug() << obj;
        if (obj) {
            obj->openPluginWin = [=](QWidget* widget, bool deleteOnClose) {
                openPluginWin(widget, deleteOnClose);
            };
            CMSPluginInterface::t_ui ui =
                obj->initialCMS(&bgmrpcClient, this, ui_cmsSubWin.widget());
            if (ui.toolboxWidget)
                ui_widPluginsToolbox->layout()->addWidget(ui.toolboxWidget);
            if (ui.pluginAction) ui_menuPlugins->addAction(ui.pluginAction);
            if (ui.pluginPage)
                ui_tbxPlugins->addItem(ui.pluginPage,
                                       ui.pluginPage->windowTitle());
            /*if (instance->metaObject()->indexOfSignal(
                    "openPluginWindow(QWidget*)")) {
                QObject::connect(instance, SIGNAL(openPluginWindow(QWidget*)),
                                 this, SLOT(openPluginWin(QWidget*)));
            }*/
        }
    }
}

SubWinBase*
BGCMSMainWin::findSubWin(int id) {
    if (id == -1)
        return &ui_cmsSubWin;
    else {
        //        bool opened = false;

        QMdiSubWindow* subWin;
        foreach (subWin, mdiArea->subWindowList()) {
            SubWinBase* _subWin_ = qobject_cast<SubWinBase*>(subWin);
            if (_subWin_->winID() == id) return _subWin_;
        }

        return nullptr;
        /*QMdiSubWindow* subWin;

        foreach (subWin, mdiArea->subWindowList()) {
            ResSubWinWidgetBase* resSubWinWidget =
                qobject_cast<ResSubWinWidgetBase*>(subWin->widget());
            if (resSubWinWidget && resSubWinWidget->resID() == id) {
                opened = true;
                //            mdiArea->setActiveSubWindow(subWin);
                break;
            }
        }

        return opened ? subWin : nullptr;*/
    }
}
