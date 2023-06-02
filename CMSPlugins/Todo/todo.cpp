/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/4/14 2022 by blueghost
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
#include "todo.h"

#include <QApplication>
#include <QInputDialog>
#include <QSettings>
#include <QStringListModel>
#include <QWindow>

Todo::Todo(QObject* parent) : QObject(parent) {}

pluginUI
Todo::initial(CMSApi* api, CMSBrowserBase* browser, const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    m_api.setCMSApi(api);

    QSettings settings;

    settings.value("contentType/todo").toList();
    if (!settings.contains("contentType/todo"))
        settings.setValue("contentType/todo", "todo");
    else
        qDebug() << "has todo type";

    ui.newNodeMenu->addAction(tr("Todo"), this, [=]() {
        bool ok;
        QString name = QInputDialog::getText(qApp->activeWindow(),
                                             tr("New Todo"), tr("Name"),
                                             QLineEdit::Normal, QString(), &ok);
        if (ok) {
            QVariant pNode = browser->listNode();
            QString resContent = "CMS_" + pNode.toString() + "_" + name;
            qDebug() << resContent;
            CallGraph::start("newTodo", this)
                ->nodes(
                    "newTodo",
                    [=](CallGraph* cg, const QVariant&) {
                        api->addNode(
                            pNode,
                            QVariantMap({ { "name", name },
                                          { "contentType", "todo" },
                                          { "content", "res:" + resContent },
                                          { "type", "R" } }),
                            cg, "end", "end");
                    })
                ->nodes("end",
                        [=](CallGraph* cg, const QVariant& data) {
                            qDebug() << data;
                            cg->toFinal();
                        })
                ->exec();
        }
    });

    QObject::connect(m_cmsBrowser, SIGNAL(pathChanged(QString)), this,
                     SLOT(dirNodeTodo()));

    QObject::connect(m_cmsBrowser, SIGNAL(nodeItemDoubleClicked(QVariantMap)),
                     this, SLOT(openTodo(QVariantMap)));

    pluginUI _ui;

    m_todoListView = new TodoListView(&m_api);
    // todoView->load("cms_1");
    m_todoListView->setObjectName("todo");
    m_todoListView->setWindowTitle(tr("Todo"));

    _ui.docker = m_todoListView;

    return _ui;
}

void
Todo::dirNodeTodo() {
    CallGraph::start("nodeInfo", this)
        ->nodes("nodeInfo",
                [=](CallGraph* cg, const QVariant&) {
                    m_api.cmsApi()->nodeInfo(m_cmsBrowser->listNode(), cg,
                                             "loadTodoList", "error");
                })
        ->nodes("loadTodoList",
                [=](CallGraph* cg, const QVariant& data) {
                    m_todoListView->load(data);
                    cg->toFinal();
                })
        ->nodes("error", [=](CallGraph* cg,
                             const QVariant& data) { cg->toFinal(); })
        ->exec();
}

void
Todo::openTodo(const QVariantMap& node) {
    if (node["contentType"] == "todo") {
        bool opened = false;
        foreach (QWindow* win, qApp->allWindows()) {
            QWidget* winWid = QWidget::find(win->winId());
            if (winWid && winWid->property("nodeID") == node["id"]) {
                win->raise();
                opened = true;
                break;
            }
        }

        if (!opened) {
            TodoListView* todoListView =
                new TodoListView(&m_api, qApp->activeWindow());
            todoListView->setWindowFlag(Qt::Window);
            todoListView->setAttribute(Qt::WA_DeleteOnClose, true);
            todoListView->setWindowTitle(tr("Todo - ") +
                                         node["title"].toString() + '[' +
                                         node["name"].toString() + ']');
            todoListView->setProperty("nodeID", node["id"]);
            todoListView->show();
            todoListView->load(node);
        }
    }
}
