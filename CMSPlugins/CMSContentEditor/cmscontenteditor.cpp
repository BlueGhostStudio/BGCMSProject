
/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/1/31 2022 by blueghost
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
#include "cmscontenteditor.h"

#include <QClipboard>
#include <QDebug>
#include <QInputDialog>
#include <QLabel>
#include <QRegularExpression>
#include <QWindow>

#include "editorwindow.h"
#include "newnodedlg.h"

CMSContentEditor::CMSContentEditor(QObject* parent) : QObject(parent) {}

pluginUI
CMSContentEditor::initial(CMSApi* api, CMSBrowserBase* browser,
                          const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);
    if (!m_settings.contains("contentType/source"))
        m_settings.setValue("contentType/source", QStringList() << "html"
                                                                << "css"
                                                                << "txt"
                                                                << "md"
                                                                << "cmp"
                                                                << "fra"
                                                                << "pkg"
                                                                << "style");

    m_sourceSubNewNodeMenu = new QMenu(tr("Source"));
    ui.newNodeMenu->addMenu(m_sourceSubNewNodeMenu);

    updateNewNodeMenu();

    QMenu* editMenu = ui.settingMenu->addMenu(tr("Editor"));

    editMenu->addAction(tr("Source Content Types"), this, [=]() {
        QString typesText;
        bool begin = true;
        foreach (const QVariant& type,
                 m_settings.value("contentType/source").toList()) {
            if (begin)
                begin = false;
            else
                typesText += ',';

            typesText += type.toString();
        }

        bool ok;
        typesText = QInputDialog::getText(
            qApp->activeWindow(), tr("Source Content Types"), tr("Types"),
            QLineEdit::Normal, typesText, &ok);

        if (ok) {
            m_settings.setValue("contentType/source", typesText.split(","));
            updateNewNodeMenu();
        }
    });

    editMenu->addSeparator();

    editMenu->addAction(tr("Syntax - html"), this, [=]() {
        QString syntaxsText;
        bool begin = true;
        foreach (const QVariant& type,
                 m_settings
                     .value("contentEditor/syntax/html",
                            QVariantList({ "html", "fra", "cmp", "pkg" }))
                     .toList()) {
            if (begin)
                begin = false;
            else
                syntaxsText += ',';

            syntaxsText += type.toString();
        }

        bool ok;
        syntaxsText = QInputDialog::getText(
            qApp->activeWindow(), tr("Syntax - html"), tr("Types"),
            QLineEdit::Normal, syntaxsText, &ok);

        if (ok)
            m_settings.setValue("contentEditor/syntax/html",
                                syntaxsText.split(","));
    });

    editMenu->addAction(tr("Syntax - markdown"), this, [=]() {
        QString syntaxsText;
        bool begin = true;
        foreach (
            const QVariant& type,
            m_settings.value("contentEditor/syntax/md", QVariantList({ "md" }))
                .toList()) {
            if (begin)
                begin = false;
            else
                syntaxsText += ',';

            syntaxsText += type.toString();
        }

        bool ok;
        syntaxsText = QInputDialog::getText(
            qApp->activeWindow(), tr("Syntax - markdown"), tr("Types"),
            QLineEdit::Normal, syntaxsText, &ok);

        if (ok)
            m_settings.setValue("contentEditor/syntax/md",
                                syntaxsText.split(","));
    });

    editMenu->addAction(tr("Syntax - css"), this, [=]() {
        QString syntaxsText;
        bool begin = true;
        foreach (const QVariant& type,
                 m_settings
                     .value("contentEditor/syntax/css",
                            QVariantList({ "css", "style" }))
                     .toList()) {
            if (begin)
                begin = false;
            else
                syntaxsText += ',';

            syntaxsText += type.toString();
        }

        bool ok;
        syntaxsText = QInputDialog::getText(
            qApp->activeWindow(), tr("Syntax - css"), tr("Types"),
            QLineEdit::Normal, syntaxsText, &ok);

        if (ok)
            m_settings.setValue("contentEditor/syntax/css",
                                syntaxsText.split(","));
    });

    QObject::connect(m_cmsBrowser, SIGNAL(nodeItemDoubleClicked(QVariantMap)),
                     this, SLOT(openNode(QVariantMap)));

    return pluginUI();
}

void
CMSContentEditor::newSourceNode(const QString& contentType,
                                const QString& content) {
    CallGraph::start("openDlg", this)
        ->nodes("openDlg",
                [=](CallGraph* cg, const QVariant&) {
                    newNodeDlg* dlg = new newNodeDlg(cg, contentType, "accept",
                                                     "", qApp->activeWindow());
                    dlg->open();
                })
        ->nodes("accept",
                [=](CallGraph* cg, const QVariant& data) {
                    /*m_api->addNode(m_cmsBrowser->listNode(), data.toMap(), cg,
                                  "end", "error");*/
                    qDebug() << m_cmsBrowser->listNode() << data;
                    QVariantMap m = data.toMap();
                    m["content"] = content;
                    m_api->addNode(m_cmsBrowser->listNode(), m, cg,
                                   "openEditor", "error");
                })
        ->nodes("error",
                [=](CallGraph* cg, const QVariant& data) {
                    // statusMessage(tr("ERROR: ") + data.toString());
                    emit logMessage(tr("ERROR: ") + data.toString());

                    cg->toFinal();
                })
        ->nodes("openEditor",
                [=](CallGraph* cg, const QVariant& data) {
                    openNode(data.toMap());

                    cg->toFinal();
                })
        ->exec();
}

void
CMSContentEditor::updateNewNodeMenu() {
    m_sourceSubNewNodeMenu->clear();
    foreach (const QVariant& type,
             m_settings.value("contentType/source").toList()) {
        QString typeText = type.toString();
        m_sourceSubNewNodeMenu->addAction(typeText, this,
                                          [=]() { newSourceNode(typeText); });
    }
}

void
CMSContentEditor::openNode(const QVariantMap& node) {
    if (node["type"] == "F") {
        QString contentType = node["contentType"].toString();
        if (m_settings.value("contentType/source")
                .toList()
                .contains(contentType)) {
            bool opened = false;
            foreach (QWindow* win, qApp->allWindows()) {
                QWidget* winWid = QWidget::find(win->winId());
                if (winWid && winWid->property("nodeID") == node["id"]) {
                    win->raise();
                    opened = true;
                    break;
                }
            };
            if (!opened) {
                EditorWindow* win = new EditorWindow(m_api);
                win->setProperty("nodeID", node["id"]);
                win->load(node, contentType);
            }
        }
    }
}
