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
#include <QSettings>
#include <QToolButton>
#include <QWindow>

EditorWindow::EditorWindow(CMSApi *api, QWidget *parent)
    : QMainWindow(parent), m_api(api), m_defaultFont(QFont("Monospace", 10)) {
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    QToolButton *showExtButton = new QToolButton(this);
    showExtButton->setCheckable(true);
    ui_twExtTab->hide();
    QObject::connect(showExtButton, &QToolButton::toggled, ui_twExtTab,
                     &QTabWidget::setVisible);
    showExtButton->setIcon(QIcon(":/drop_down.png"));
    ui_menubar->setCornerWidget(showExtButton);

    QObject::connect(
        ui_contentEditor, &QsciScintilla::modificationChanged, this, [=]() {
            setWindowTitle(m_windowTitle + (isModified() ? "*" : ""));
            action_save->setEnabled(isModified());
        });
    QObject::connect(
        ui_pteSummary, &QPlainTextEdit::modificationChanged, this, [=]() {
            setWindowTitle(m_windowTitle + (isModified() ? "*" : ""));
            action_save->setEnabled(isModified());
        });
    QObject::connect(
        ui_pteExtData, &QPlainTextEdit::modificationChanged, this, [=]() {
            setWindowTitle(m_windowTitle + (isModified() ? "*" : ""));
            action_save->setEnabled(isModified());
        });

    QClipboard *clipboard = QGuiApplication::clipboard();
    action_paste->setEnabled(clipboard->mimeData()->hasText());
    QObject::connect(clipboard, &QClipboard::dataChanged, this, [=]() {
        action_paste->setEnabled(clipboard->mimeData()->hasText());
    });

    ui_widFind->setVisible(false);
    QToolButton *showFindButton = new QToolButton(this);
    showFindButton->setText(tr("Find/Replace"));
    showFindButton->setCheckable(true);
    ui_statusbar->addPermanentWidget(showFindButton);
    QObject::connect(showFindButton, &QToolButton::toggled, ui_widFind,
                     &QWidget::setVisible);
    QObject::connect(this, &EditorWindow::showFindPanel, this,
                     [=]() { showFindButton->setChecked(true); });

    ui_contentEditor->installEventFilter(this);

    initialEditor();
}

EditorWindow::~EditorWindow() { qDebug() << "destory"; }

void
EditorWindow::load(const QVariantMap &_node, const QString &type) {
    CallGraph::start("getNode", this)
        ->nodes("getNode",
                [=](CallGraph *cg, const QVariant &data) {
                    m_api->node(_node["id"], cg, "openNode", "error");
                })
        ->nodes(
            "openNode",
            [=](CallGraph *cg, const QVariant &data) {
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

                /*if (type.contains(QRegularExpression("^(html|pkg|fra|cmp)$")))
                    ui_contentEditor->setLexer(&m_lexerHTML);
                else if (type.contains(QRegularExpression("^(css|style)$")))
                    ui_contentEditor->setLexer(&m_lexerCSS);
                else if (type == "md")
                    ui_contentEditor->setLexer(&m_lexerMarkdown);*/

                QSettings settings;
                if (settings
                        .value("contentEditor/syntax/html",
                               QVariantList(
                                   { "html", "fra", "cmp", "pkg", "elm" }))
                        .toList()
                        .contains(type))
                    ui_contentEditor->setLexer(&m_lexerHTML);
                else if (settings
                             .value("contentEditor/syntax/md",
                                    QVariantList({ "md" }))
                             .toList()
                             .contains(type))
                    ui_contentEditor->setLexer(&m_lexerMarkdown);
                else if (settings
                             .value("contentEditor/syntax/css",
                                    QVariantList({ "css", "style" }))
                             .toList()
                             .contains(type))
                    ui_contentEditor->setLexer(&m_lexerCSS);

                m_node = data.toMap();
                ui_contentEditor->setText(m_node["content"].toString());
                ui_contentEditor->setModified(false);
                ui_pteSummary->setPlainText(m_node["summary"].toString());
                ui_pteExtData->setPlainText(m_node["extData"].toString());

                action_undo->setEnabled(false);
                action_redo->setEnabled(false);
                action_save->setEnabled(false);
                show();

                m_api->nodePath(m_node["id"], cg, "setWinTitle");
            })
        ->nodes("setWinTitle",
                [=](CallGraph *cg, const QVariant &data) {
                    m_windowTitle =
                        tr("BGCMS") + " - " + data.toMap()["str"].toString();
                    setWindowTitle(m_windowTitle);
                    cg->toFinal();
                })
        ->nodes("error",
                [=](CallGraph *cg, const QVariant &data) {
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

    QObject::connect(ui_contentEditor, &QsciScintilla::textChanged, [=]() {
        action_undo->setEnabled(ui_contentEditor->isUndoAvailable());
        action_redo->setEnabled(ui_contentEditor->isRedoAvailable());
    });

    m_lexerHTML.setAutoIndentStyle(QsciScintilla::AiClosing |
                                   QsciScintilla::AiOpening);
    m_lexerHTML.setFont(m_defaultFont);
    m_lexerHTML.setScriptsStyled(true);
    m_lexerHTML.setFoldCompact(false);
    m_lexerHTML.setFoldPreprocessor(true);
    m_lexerHTML.setDefaultPaper(QColor("white"));
    m_lexerHTML.setPaper(QColor("white"));
    m_lexerHTML.setDefaultColor(QColor("black"));

    m_lexerCSS.setAutoIndentStyle(QsciScintilla::AiClosing |
                                  QsciScintilla::AiOpening);
    m_lexerCSS.setFont(m_defaultFont);
    // m_lexerCSS.setHSSLanguage(true);
    m_lexerCSS.setLessLanguage(true);
    m_lexerCSS.setFoldCompact(false);
    m_lexerCSS.setDefaultPaper(QColor("white"));
    m_lexerCSS.setPaper(QColor("white"));
    m_lexerCSS.setDefaultColor(QColor("black"));

    m_lexerMarkdown.setFont(m_defaultFont);
    m_lexerMarkdown.setDefaultPaper(QColor("white"));
    m_lexerMarkdown.setPaper(QColor("white"));
    m_lexerMarkdown.setColor(QColor("black"), QsciLexerMarkdown::Default);
    m_lexerMarkdown.setDefaultColor(QColor("black"));

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
EditorWindow::setSyntaxHighlight(const QString &type) {}

bool
EditorWindow::isModified() const {
    return ui_contentEditor->isModified() ||
           ui_pteSummary->document()->isModified() ||
           ui_pteExtData->document()->isModified();
}

void
EditorWindow::save(CallGraph *pcg, const QString &to, const QString &error) {
    CallGraph::start("save", this)
        ->nodes("save",
                [=](CallGraph *cg, const QVariant &data) {
                    // qDebug() << ui_contentEditor->text();
                    QString content = ui_contentEditor->text();
                    content.remove(QRegularExpression(
                        "\\x20+$", QRegularExpression::MultilineOption));

                    m_api->updateNode(
                        m_node["id"],
                        QVariantMap(
                            { { "content", content },
                              { "summary", ui_pteSummary->toPlainText() },
                              { "extData", ui_pteExtData->toPlainText() } }),
                        cg, "updateNode", "error");
                })
        ->nodes(
            "updateNode",
            [=](CallGraph *cg, const QVariant &data) {
                m_node = data.toMap();
                ui_contentEditor->setModified(false);
                ui_pteSummary->document()->setModified(false);
                ui_pteExtData->document()->setModified(false);
                if (pcg /*.isNull()*/ && !to.isEmpty()) pcg->to(to);
                cg->toFinal();
            },
            "error",
            [=](CallGraph *cg, const QVariant &data) {
                if (pcg /*.isNull()*/ && !error.isEmpty()) pcg->to(error);
                cg->toFinal();
            })
        ->exec();
}

void
EditorWindow::find() {
    ui_contentEditor->findFirst(ui_leFindText->text(), true, false, true, true);
    ui_contentEditor->setFocus();
}

void
EditorWindow::findBackwards() {
    if (ui_contentEditor->hasSelectedText())
        ui_contentEditor->SendScintilla(
            QsciScintilla::SCI_GOTOPOS,
            ui_contentEditor->SendScintilla(
                QsciScintilla::SCI_GETSELECTIONSTART));

    ui_contentEditor->findFirst(ui_leFindText->text(), true, false, true, true,
                                false);
    ui_contentEditor->setFocus();
}

void
EditorWindow::replace() {
    ui_contentEditor->replace(ui_leReplaceText->text());
    ui_contentEditor->findNext();
}

void
EditorWindow::closeEvent(QCloseEvent *event) {
    if (isModified()) {
        QMessageBox::StandardButton btn = QMessageBox::question(
            this, tr("Content has modified"), tr("Save it?"),
            QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);
        if (btn == QMessageBox::Save) {
            CallGraph::start("save", this)
                ->nodes("save", [=](CallGraph *cg,
                                    const QVariant &) { save(cg, "saved"); })
                ->nodes("saved",
                        [=](CallGraph *cg, const QVariant &) {
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

bool
EditorWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui_contentEditor && event->type() == QEvent::KeyPress) {
        QKeyEvent *kev = static_cast<QKeyEvent *>(event);
        if (kev->matches(QKeySequence::Find)) {
            // ui_widFind->show();
            emit showFindPanel();
            ui_leFindText->setText(ui_contentEditor->selectedText());
            ui_leFindText->setFocus();
            return true;
        } else
            return false;
    } else
        return QMainWindow::eventFilter(watched, event);
}
