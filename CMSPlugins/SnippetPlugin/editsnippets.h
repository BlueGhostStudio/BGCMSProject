#ifndef EDITSNIPPETS_H
#define EDITSNIPPETS_H

#include <QJsonDocument>

#include "snippetplugin.h"
#include "ui_editsnippets.h"

class EditSnippets : public QDialog, private Ui::EditSnippets {
    Q_OBJECT

private:
    struct t_snippetBook {
        QString fileName;
        bool modified = false;
        // QJsonDocument snippetsDefine;
    };

public:
    explicit EditSnippets(QWidget* parent = nullptr);

private slots:
    void loadSnippetBookList();
    void addSnippetBook();
    void deleteSnippetBook();

    void loadSnippetBook(QListWidgetItem* item);
    void loadSnippetItem(int row);
    void addSnippet();
    void deleteSnippet();

    void updateBookLabel(const QString&);

    void updateSnippetLabel(const QString& label);
    void updateSnippetIcon(const QString& iconPath);
    void updateSnippetTex();
    void updateSnippetPos(int pos);
    void selectIcon();

    void save();

    void accept();

private:
    t_snippetBook m_currentSnippetBook;
    QString m_snippetsPath;
};

#endif  // EDITSNIPPETS_H
