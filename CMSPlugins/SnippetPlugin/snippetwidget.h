#ifndef SNIPPETWIDGET_H
#define SNIPPETWIDGET_H

#include <Qsci/qsciscintilla.h>

#include "snippetplugin.h"
#include "ui_snippetwidget.h"

class SnippetWidget : public QWidget, private Ui::SnippetWidget {
    Q_OBJECT

public:
    explicit SnippetWidget(QsciScintilla* editor,
                           QMap<QString, QList<t_snippet>>* snippetrs,
                           QWidget* parent = nullptr);

signals:
    void editSnipperts();

public slots:
    void loadSnippets();

private slots:
    void loadSnippets(const QString& book);
    void insertSnippet(QListWidgetItem* item);

private:
    void movePos(int pos);

private:
    QsciScintilla* m_targetEditor;
    QMap<QString, QList<t_snippet>>* m_snippets;
};

#endif  // SNIPPETWIDGET_H
