#include "resourceeditor.h"

#include <global.h>

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QtDebug>

ResourceEditor::ResourceEditor(QWidget* parent)
    : ResSubWinWidgetBase(parent),
      ui_editToolbar(parent),
      m_defaultFont(QFont("Monospace", 12)) {
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    ui_editorLayout->insertWidget(0, &ui_editToolbar);
    ui_editToolbar.addAction(actionSave);
    ui_editToolbar.addAction(actionFind);

    ui_editToolbar.addAction(actionPlugins);
    // ui_twPlugins->setVisible(actionPlugins->isChecked());
    /*QObject::connect(actionPlugins, &QAction::toggled, ui_widPlugins,
                     &QWidget::setVisible);*/
    ui_widPlugins->setVisible(actionPlugins->isChecked());

    QObject::connect(actionFind, &QAction::triggered, this, [=](bool checked) {
        if (checked) ui_leFindText->setFocus();
    });

    ui_splEditor->setSizes({ qFloor(width() * 0.8), qFloor(width() * 0.2) });
}

void
ResourceEditor::initialEditor(QMainWindow* mainWin, QList<QObject*>& plugins) {
    qsciResourceEditor->setWrapMode(QsciScintilla::WrapWord);
    qsciResourceEditor->setAutoCompletionSource(QsciScintilla::AcsAll);
    qsciResourceEditor->setAutoCompletionThreshold(1);
    qsciResourceEditor->setAutoIndent(true);
    qsciResourceEditor->setIndentationsUseTabs(false);
    qsciResourceEditor->setTabIndents(true);
    qsciResourceEditor->setBackspaceUnindents(true);
    qsciResourceEditor->setIndentationWidth(4);
    qsciResourceEditor->setTabWidth(4);
    qsciResourceEditor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    qsciResourceEditor->setMarginLineNumbers(0, true);
    qsciResourceEditor->setMarginWidth(0, "01234");
    qsciResourceEditor->setFolding(QsciScintilla::CircledTreeFoldStyle);

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

    QObject::connect(qsciResourceEditor, &QsciScintilla::modificationChanged,
                     actionSave, &QAction::setEnabled);
    QObject::connect(qsciResourceEditor, &QsciScintilla::modificationChanged,
                     this, &ResourceEditor::setWindow);
    QObject::connect(
        qsciResourceEditor, &QsciScintilla::modificationChanged, this,
        [=](bool m) { emit hasModified(m_resource["id"].toInt(), m); });
    emit qsciResourceEditor->modificationChanged(false);

    ui_widFind->setVisible(false);

    /*for (EditorPluginInterface *interface, plugins) {
        interface->initial(ui_editToolbar, qsciResourceEditor);
    }*/
    foreach (QObject* interface, plugins) {
        EditorPluginInterface* object =
            qobject_cast<EditorPluginInterface*>(interface);
        if (interface && object) {
            QWidget* widget = object->initialEditor(&bgmrpcClient, mainWin,
                                                    qsciResourceEditor);
            widget->setParent(ui_splPlugins);
            ui_splPlugins->addWidget(widget);
            // ui_twPlugins->addTab(widget, widget->windowTitle());
            m_editorPlugins.append(
                QPair<EditorPluginInterface*, QWidget*>(object, widget));
        }
    }
    // ui_twPlugins->hide();
}

bool
ResourceEditor::modified() {
    return qsciResourceEditor->isModified();
}

void
ResourceEditor::openResource(const QVariantMap& resource) {
    ResSubWinWidgetBase::openResource(resource);
    //    setWindowTitle(resource["title"].toString());
    //    setWindow();
    QString type = m_resource["type"].toString();
    if (type == "A")
        qsciResourceEditor->setLexer(&m_lexerMarkdown);
    else if (type == "C")
        qsciResourceEditor->setLexer(&m_lexerHTML);
    else if (type == "S")
        qsciResourceEditor->setLexer(&m_lexerCSS);

    qsciResourceEditor->SendScintilla(QsciScintilla::SCI_STYLESETFONT,
                                      QsciScintilla::STYLE_BRACELIGHT,
                                      m_defaultFont.family().toLatin1().data());
    qsciResourceEditor->SendScintilla(QsciScintilla::SCI_STYLESETFONT,
                                      QsciScintilla::STYLE_BRACEBAD,
                                      m_defaultFont.family().toLatin1().data());

    qsciResourceEditor->SendScintilla(QsciScintilla::SCI_STYLESETSIZE,
                                      QsciScintilla::STYLE_BRACELIGHT,
                                      m_defaultFont.pointSize());
    qsciResourceEditor->SendScintilla(QsciScintilla::SCI_STYLESETSIZE,
                                      QsciScintilla::STYLE_BRACEBAD,
                                      m_defaultFont.pointSize());

    bgmrpcClient.callMethod("CMS", "resourceContent", { m_resource["id"] })
        ->then([=](const QVariant& ret) {
            QVariantMap map = ret.toList()[0].toMap();
            qsciResourceEditor->setText(map["content"].toString());
            qsciResourceEditor->setModified(false);

            /*for(int i = 0; i < m_editorPlugins.length(); i++) {
                QPair<EditorPluginInterface*, QWidget*> item =
            m_editorPlugins[i];
            }*/
            QList<QPair<EditorPluginInterface*, QWidget*>>::iterator it;
            for (it = m_editorPlugins.begin(); it != m_editorPlugins.end();
                 it++) {
                it->first->setResource(resource, it->second);
            }
        });
}

void
ResourceEditor::saveResource(bool saveBeforeClose) {
    QString content = qsciResourceEditor->text();
    if (m_resource["type"] != "A")
        content.remove(
            QRegularExpression("\\x20+$", QRegularExpression::MultilineOption));

    bgmrpcClient
        .callMethod(
            "CMS", "updateResource",
            { m_resource["id"], QVariantMap({ { "content", content } }) })
        ->then([=](const QVariant&) {
            qsciResourceEditor->setModified(false);
            if (saveBeforeClose)
                qobject_cast<QMdiSubWindow*>(parent())->close();
        });
}

void
ResourceEditor::closeAboutDeleted() {
    qsciResourceEditor->setModified(false);
    ResSubWinWidgetBase::closeAboutDeleted();
}

void
ResourceEditor::find() {
    //    qsciResourceEditor->findFirst(ui_le)
    qsciResourceEditor->findFirst(ui_leFindText->text(), true, false, true,
                                  true);
    qsciResourceEditor->setFocus();
}

void
ResourceEditor::findBackwards() {
    /*qsciResourceEditor->findFirst(ui_leFindText->text(), true, false, true,
                                  true, false);
    qsciResourceEditor->findNext();
    qDebug() << qsciResourceEditor->SendScintilla(
        QsciScintilla::SCI_GETSELECTIONEND);*/
    if (qsciResourceEditor->hasSelectedText())
        qsciResourceEditor->SendScintilla(
            QsciScintilla::SCI_GOTOPOS,
            qsciResourceEditor->SendScintilla(
                QsciScintilla::SCI_GETSELECTIONSTART));

    qsciResourceEditor->findFirst(ui_leFindText->text(), true, false, true,
                                  true, false);
    qsciResourceEditor->setFocus();
}

void
ResourceEditor::replace() {
    qsciResourceEditor->replace(ui_leReplaceText->text());
    qsciResourceEditor->findNext();
}

void
ResourceEditor::closeEvent(QCloseEvent* event) {
    /*if (!qsciResourceEditor->isModified())
        event->accept();*/
    if (bgmrpcClient.isConnected() && qsciResourceEditor->isModified()) {
        QMessageBox::StandardButton btn = QMessageBox::question(
            nullptr, tr("Resource has modified"),
            tr("Do you want to save the modified document?"));
        if (btn == QMessageBox::No) {
            qsciResourceEditor->setModified(false);
            QTimer::singleShot(
                50, [&]() { qobject_cast<QMdiSubWindow*>(parent())->close(); });
        } else {
            saveResource(true);
        }
        event->ignore();
    }
}

/*void
ResourceEditor::setWindow() {
    setWindowTitle(m_resource["title"].toString());
    QString type = m_resource["type"].toString();

    qobject_cast<QMdiSubWindow*>(parent())->setWindowIcon(
        resIcon(m_resource["type"].toString()));
}*/
