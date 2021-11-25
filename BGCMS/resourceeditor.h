#ifndef RESOURCEEDITOR_H
#define RESOURCEEDITOR_H

#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexerxml.h>

#include <QCloseEvent>
#include <QToolBar>

#include "cmswidget.h"
#include "editorplugininterface.h"
#include "ressubwinwidgetbase.h"
#include "ui_resourceeditor.h"

class ResourceEditor : public ResSubWinWidgetBase, private Ui::ResourceEditor {
    Q_OBJECT

public:
    explicit ResourceEditor(QWidget* parent = nullptr);

    void initialEditor(QMainWindow* mainWin, QList<QObject*>& plugins);
    bool modified() override;

signals:
    void hasModified(int res, bool m);

public slots:
    void openResource(const QVariantMap& resource) override;
    void saveResource(bool saveBeforeClose = false);
    void closeAboutDeleted() override;

    void find();
    void findBackwards();
    void replace();

protected:
    void closeEvent(QCloseEvent* event) override;

    /*private:
        void setWindow();*/

private:
    QToolBar ui_editToolbar;

    QsciLexerMarkdown m_lexerMarkdown;
    //    QsciLexerHTML m_lexerHTML;
    QsciLexerXML m_lexerHTML;
    QsciLexerCSS m_lexerCSS;

    //    QVariantMap m_resource;
    QFont m_defaultFont;
    QList<QPair<EditorPluginInterface*, QWidget*>> m_editorPlugins;
};

#endif  // RESOURCEEDITOR_H
