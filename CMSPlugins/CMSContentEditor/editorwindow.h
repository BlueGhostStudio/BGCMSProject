/*****************************************************************************
 * editorwindow.h
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
#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexerxml.h>
#include <bgmrpcclient.h>

#include <QMimeDatabase>

#include "../../BGCMS/cmsapi.h"
#include "ui_editorwindow.h"

class EditorWindow : public QMainWindow, private Ui::EditorWindow {
    Q_OBJECT

public:
    explicit EditorWindow(CMSApi* api, QWidget* parent = nullptr);
    ~EditorWindow();

    void load(const QVariantMap& _node, const QString& type);

private:
    void initialEditor();
    void setSyntaxHighlight(const QString& type);
    bool isModified() const;

private slots:
    void save(QPointer<CallGraph> pcg = nullptr, const QString& to = QString(),
              const QString& error = QString());

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QVariantMap m_node;
    CMSApi* m_api;
    QMimeDatabase m_mimeDB;

    QFont m_defaultFont;
    QsciLexerMarkdown m_lexerMarkdown;
    //    QsciLexerHTML m_lexerHTML;
    QsciLexerXML m_lexerHTML;
    QsciLexerCSS m_lexerCSS;
};

#endif  // EDITORWINDOW_H
