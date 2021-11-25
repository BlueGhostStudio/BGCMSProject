#include "snippetplugin.h"

#include <QLabel>
#include <QStandardPaths>
#include <QtDebug>

#include "editsnippets.h"
#include "snippetstoolbox.h"
#include "snippetwidget.h"

QString snippetsRelativePath = "/Snippets";

SnippetPlugin::SnippetPlugin(QObject* parent) : QObject(parent) {
    QString snippetsPath =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) +
        snippetsRelativePath;

    QDir dir;
    if (!dir.exists(snippetsPath)) {
        dir.mkpath(snippetsPath + "/icons");
        QDir qrcDir(":/snippets");
        foreach (const QFileInfo& fileInfo, qrcDir.entryInfoList(QDir::Files)) {
            QFile::copy(fileInfo.absoluteFilePath(),
                        snippetsPath + '/' + fileInfo.fileName());
        }
        qrcDir.setPath(":/snippets/icons");
        foreach (const QFileInfo& fileInfo, qrcDir.entryInfoList(QDir::Files)) {
            QFile::copy(fileInfo.absoluteFilePath(),
                        snippetsPath + "/icons/" + fileInfo.fileName());
        }
    }
    loadSnippets();
}

QWidget*
SnippetPlugin::initialEditor(NS_BGMRPCClient::BGMRPCClient* /*bgmrpcClient*/,
                             QMainWindow* /*mainWin*/,
                             QsciScintilla* qsciEditor) {
    // qDebug() << bgmrpcClient->isConnected();
    SnippetWidget* widget = new SnippetWidget(qsciEditor, &m_snippets);
    widget->setWindowTitle("Snippets");
    // pluginTabWidget->addTab(widget, tr("Snippet"));
    // pluginTabWidget->addTab(widget, tr("Snippet"));
    QObject::connect(widget, &SnippetWidget::editSnipperts, this,
                     &SnippetPlugin::editSnippets);
    QObject::connect(this, &SnippetPlugin::snippetsUpdated, widget,
                     QOverload<>::of(&SnippetWidget::loadSnippets));
    return widget;
}

CMSPluginInterface::t_ui
SnippetPlugin::initialCMS(NS_BGMRPCClient::BGMRPCClient*,
                          QMainWindow* /*mainWin*/, QObject* /*cmsWidget*/) {
    qDebug() << "initialCMS";
    QAction* action = new QAction(tr("Edit Snippets"));
    // QPushButton* editButton = new QPushButton(action->text());
    SnippetsToolbox* toolBox = new SnippetsToolbox;

    /*QPlainTextEdit* editor = new QPlainTextEdit;
    editor->setWindowTitle("test plugin");*/

    // pluginsLayout->addWidget(editButton);
    t_ui ui;
    ui.toolboxWidget = toolBox;
    ui.pluginAction = action;
    // ui.pluginPage = editor;

    QObject::connect(action, &QAction::triggered, this,
                     &SnippetPlugin::editSnippets);
    QObject::connect(toolBox, &SnippetsToolbox::editSnippets, this,
                     [=]() { action->trigger(); });

    // clang-format off
    /*QObject::connect(cmsWidget, SIGNAL(locationChanged(int,int,int,int)),
                     this, SLOT(onLocationChanged(int,int,int,int)));*/
    // clang-format on

    return ui;
}

void
SnippetPlugin::editSnippets() {
    EditSnippets* editor = new EditSnippets;
    editor->show();
    QObject::connect(editor, &EditSnippets::finished, this, [=]() {
        loadSnippets();
        emit snippetsUpdated();
    });
}

/*void
SnippetPlugin::onLocationChanged(int type, int cID, int cateID, int resID) {
    qDebug() << type << cID << cateID << resID;
}*/

void
SnippetPlugin::loadSnippets() {
    m_snippets.clear();
    foreach (QString path, QStandardPaths::standardLocations(
                               QStandardPaths::AppLocalDataLocation)) {
        path += snippetsRelativePath;
        foreach (const QFileInfo& snippetFile,
                 QDir(path).entryInfoList({ "*.json" }, QDir::Files)) {
            loadSnippets(snippetFile);
        }
    }
}

void
SnippetPlugin::loadSnippets(const QFileInfo& fileInfo) {
    QFile file(fileInfo.absoluteFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QVariantMap json =
            QJsonDocument::fromJson(file.readAll()).toVariant().toMap();

        QString path = fileInfo.absolutePath();
        QString label = json["label"].toString();
        if (label.length() > 0) {
            if (!m_snippets.contains(label))
                m_snippets[label] = QList<t_snippet>();

            QList<t_snippet>& list = m_snippets[label];
            foreach (const QVariant& var, json["snippets"].toList()) {
                QVariantMap map = var.toMap();

                QIcon icon;
                if (map.contains("icon")) {
                    QString iconFile = map["icon"].toString();
                    if (QFileInfo(iconFile).isRelative())
                        icon.addFile(path + "/icons/" + map["icon"].toString());
                    else
                        icon.addFile(iconFile);
                }

                QString tex = map["tex"].toString();

                int pos;
                if (map.contains("pos"))
                    pos = map["pos"].toInt();
                else
                    pos = tex.length();

                t_snippet snippet = { map["label"].toString(), icon,
                                      map["tex"].toString(), pos };
                list.append(snippet);
            }
        }
    }
    file.close();
}
