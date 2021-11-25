#include "snippetwidget.h"

#include <QtDebug>

SnippetWidget::SnippetWidget(QsciScintilla* editor,
                             QMap<QString, QList<t_snippet> >* snippets,
                             QWidget* parent)
    : QWidget(parent), m_targetEditor(editor), m_snippets(snippets) {
    setupUi(this);

    loadSnippets();

    QObject::connect(ui_pbEdit, &QPushButton::clicked, this,
                     &SnippetWidget::editSnipperts);
}

void
SnippetWidget::loadSnippets() {
    ui_cbSnipptBook->clear();
    QStringList keys = m_snippets->keys();
    foreach (const QString& label, keys) { ui_cbSnipptBook->addItem(label); }
    loadSnippets(ui_cbSnipptBook->currentText());
}

void
SnippetWidget::loadSnippets(const QString& book) {
    ui_lwSnippets->clear();
    if (m_snippets->contains(book)) {
        foreach (const t_snippet& snippet, (*m_snippets)[book]) {
            QListWidgetItem* item = new QListWidgetItem(ui_lwSnippets);
            item->setIcon(snippet.icon);
            item->setText(snippet.label);

            item->setData(Qt::UserRole + 1, snippet.tex);
            item->setData(Qt::UserRole + 2, snippet.pos);

            ui_lwSnippets->addItem(item);
        }
    }
}

void
SnippetWidget::insertSnippet(QListWidgetItem* item) {
    QString tex = item->data(Qt::UserRole + 1).toString();
    int pos = item->data(Qt::UserRole + 2).toInt();
    m_targetEditor->insert(tex);
    movePos(pos);
}

void
SnippetWidget::movePos(int pos) {
    m_targetEditor->SendScintilla(
        QsciScintilla::SCI_GOTOPOS,
        m_targetEditor->SendScintilla(QsciScintilla::SCI_GETCURRENTPOS) + pos);
}
