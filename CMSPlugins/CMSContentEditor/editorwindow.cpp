/*****************************************************************************
 * editorwindow.cpp
 *
 * Created: 2022/2/3 2022 by blueghost
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
#include "editorwindow.h"

#include <callgraph.h>

#include <QClipboard>
#include <QCloseEvent>
#include <QMessageBox>
#include <QToolButton>

EditorWindow::EditorWindow(CMSApi* api, QWidget* parent)
    : QMainWindow(parent), m_api(api), m_defaultFont(QFont("Monospace", 12)) {
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    QToolButton* showExtButton = new QToolButton(this);
    showExtButton->setCheckable(true);
    ui_twExtTab->hide();
    QObject::connect(showExtButton, &QToolButton::toggled, ui_twExtTab,
                     &QTabWidget::setVisible);
    showExtButton->setIcon(QIcon(":/drop_down.png"));
    ui_menubar->setCornerWidget(showExtButton);

    action_save->setEnabled(false);
    QObject::connect(ui_contentEditor, &QsciScintilla::modificationChanged,
                     this, [=]() { action_save->setEnabled(isModified()); });
    QObject::connect(ui_pteSummary, &QPlainTextEdit::modificationChanged, this,
                     [=]() { action_save->setEnabled(isModified()); });
    QObject::connect(ui_pteExtData, &QPlainTextEdit::modificationChanged, this,
                     [=]() { action_save->setEnabled(isModified()); });

    QClipboard* clipboard = QGuiApplication::clipboard();
    action_paste->setEnabled(clipboard->mimeData()->hasText());
    QObject::connect(clipboard, &QClipboard::dataChanged, this, [=]() {
        action_paste->setEnabled(clipboard->mimeData()->hasText());
    });

    initialEditor();
}

EditorWindow::~EditorWindow() { qDebug() << "destory"; }

void
EditorWindow::load(const QVariantMap& _node, const QString& type) {
    CallGraph::start("getNode", this)
        ->nodes("getNode",
                [=](QPointer<CallGraph> cg, const QVariant& data) {
                    m_api->node(_node["id"], cg, "openNode", "error");
                })
        ->nodes(
            "openNode",
            [=](QPointer<CallGraph> cg, const QVariant& data) {
                ui_contentEditor->SendScintilla(
                    QsciScintilla::SCI_STYLESETFONT,
                    QsciScintilla::STYLE_BRACELIGHT,
                    m_defaultFont.family().toLatin1().data());
                ui_contentEditor->SendScintilla(
                    QsciScintilla::SCI_STYLESETFONT,
                    QsciScintilla::STYLE_BRACEBAD,
                    m_defaultFont.family().toLatin1().data());

                ui_contentEditor->SendScintilla(QsciScintilla::SCI_STYLESETSIZE,
                                                QsciScintilla::STYLE_BRACELIGHT,
                                                m_defaultFont.pointSize());
                ui_contentEditor->SendScintilla(QsciScintilla::SCI_STYLESETSIZE,
                                                QsciScintilla::STYLE_BRACEBAD,
                                                m_defaultFont.pointSize());

                if (type.contains(QRegularExpression("^(html|pkg|fra|cmp)$")))
                    ui_contentEditor->setLexer(&m_lexerHTML);
                else if (type.contains(QRegularExpression("^(css|style)$")))
                    ui_contentEditor->setLexer(&m_lexerCSS);
                else if (type == "md")
                    ui_contentEditor->setLexer(&m_lexerMarkdown);

                m_node = data.toMap();
                ui_contentEditor->setText(m_node["content"].toString());
                ui_contentEditor->setModified(false);
                ui_pteSummary->setPlainText(m_node["summary"].toString());
                ui_pteExtData->setPlainText(m_node["extData"].toString());
                show();

                m_api->nodePath(m_node["id"], cg, "setWinTitle");
            })
        ->nodes("setWinTitle",
                [=](QPointer<CallGraph> cg, const QVariant& data) {
                    setWindowTitle(tr("BGCMS") + " - " +
                                   data.toMap()["str"].toString());
                    cg->toFinal();
                })
        ->nodes("error",
                [=](QPointer<CallGraph> cg, const QVariant& data) {
                    qDebug() << data;
                    cg->toFinal();
                })
        ->exec();
}

void
EditorWindow::initialEditor() {
    ui_contentEditor->setWrapMode(QsciScintilla::WrapWord);
    ui_contentEditor->setAutoCompletionSource(QsciScintilla::AcsAll);
    ui_contentEditor->setAutoCompletionThreshold(1);
    ui_contentEditor->setAutoIndent(true);
    ui_contentEditor->setIndentationsUseTabs(false);
    ui_contentEditor->setTabIndents(true);
    ui_contentEditor->setBackspaceUnindents(true);
    ui_contentEditor->setIndentationWidth(4);
    ui_contentEditor->setTabWidth(4);
    ui_contentEditor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    ui_contentEditor->setMarginLineNumbers(0, true);
    ui_contentEditor->setMarginWidth(0, "01234");
    ui_contentEditor->setFolding(QsciScintilla::CircledTreeFoldStyle);

    m_lexerHTML.setAutoIndentStyle(QsciScintilla::AiClosing |
                                   QsciScintilla::AiOpening);
    m_lexerHTML.setFont(m_defaultFont);
    m_lexerHTML.setScriptsStyled(true);
    //    m_lexerHTML.setFoldCompact(false);

    m_lexerCSS.setAutoIndentStyle(QsciScintilla::AiClosing |
                                  QsciScintilla::AiOpening);
    m_lexerCSS.setFont(m_defaultFont);
    m_lexerCSS.setHSSLanguage(true);
    m_lexerCSS.setFoldCompact(false);

    m_lexerMarkdown.setFont(m_defaultFont);

    /*QObject::connect(ui_contentEditor, &QsciScintilla::modificationChanged,
                     actionSave, &QAction::setEnabled);
    QObject::connect(ui_contentEditor, &QsciScintilla::modificationChanged,
                     this, &ResourceEditor::setWindow);
    QObject::connect(
        ui_contentEditor, &QsciScintilla::modificationChanged, this,
        [=](bool m) { emit hasModified(m_resource["id"].toInt(), m); });
    emit ui_contentEditor->modificationChanged(false);

    ui_widFind->setVisible(false);*/
}

void
EditorWindow::setSyntaxHighlight(const QString& type) {}

bool
EditorWindow::isModified() const {
    return ui_contentEditor->isModified() ||
           ui_pteSummary->document()->isModified() ||
           ui_pteExtData->document()->isModified();
}

void
EditorWindow::save(QPointer<CallGraph> pcg, const QString& to,
                   const QString& error) {
    CallGraph::start("save", this)
        ->nodes("save",
                [=](QPointer<CallGraph> cg, const QVariant& data) {
                    // qDebug() << ui_contentEditor->text();
                    m_api->updateNode(
                        m_node["id"],
                        QVariantMap(
                            { { "content", ui_contentEditor->text() },
                              { "summary", ui_pteSummary->toPlainText() },
                              { "extData", ui_pteExtData->toPlainText() } }),
                        cg, "updateNode", "error");
                })
        ->nodes(
            "updateNode",
            [=](QPointer<CallGraph> cg, const QVariant& data) {
                m_node = data.toMap();
                ui_contentEditor->setModified(false);
                ui_pteSummary->document()->setModified(false);
                ui_pteExtData->document()->setModified(false);
                if (!pcg.isNull() && !to.isEmpty()) pcg->to(to);
                cg->toFinal();
            },
            "error",
            [=](QPointer<CallGraph> cg, const QVariant& data) {
                if (!pcg.isNull() && !error.isEmpty()) pcg->to(error);
                cg->toFinal();
            })
        ->exec();
}

void
EditorWindow::closeEvent(QCloseEvent* event) {
    if (isModified()) {
        QMessageBox::StandardButton btn = QMessageBox::question(
            this, tr("Content has modified"), tr("Save it?"),
            QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);
        if (btn == QMessageBox::Save) {
            CallGraph::start("save", this)
                ->nodes("save", [=](QPointer<CallGraph> cg,
                                    const QVariant&) { save(cg, "saved"); })
                ->nodes("saved",
                        [=](QPointer<CallGraph> cg, const QVariant&) {
                            close();
                            cg->toFinal();
                        })
                ->exec();
            event->ignore();
        } else if (btn == QMessageBox::No)
            event->accept();
        else
            event->ignore();
    } else
        event->accept();
}
