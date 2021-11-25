#include "cmswebplugin.h"

#include <QLineEdit>
#include <QRegularExpression>
#include <QWebEngineView>

#include "cmswebtoolbox.h"

CMSWebPlugin::CMSWebPlugin(QObject* parent) : QObject(parent) {
    // m_CMSWeb.setUrl(QUrl("http://bgstudio.tk/"));
}

QWidget*
CMSWebPlugin::initialEditor(NS_BGMRPCClient::BGMRPCClient* /*bgmrpcClient*/,
                            QMainWindow* /*mainWin*/,
                            QsciScintilla* /*qsciEditor*/) {
    /*QLineEdit* lineEdit = new QLineEdit;
    lineEdit->setWindowTitle(tr("Preview"));
    return lineEdit;*/

    QWebEngineView* previewWidget = new QWebEngineView;
    previewWidget->setWindowTitle(tr("Preview"));

    return previewWidget;
}

CMSPluginInterface::t_ui
CMSWebPlugin::initialCMS(NS_BGMRPCClient::BGMRPCClient* /*bgmrpcClient*/,
                         QMainWindow* mainWin, QObject* cmsWidget) {
    QObject::connect(this, SIGNAL(openCurrentResource(int)), cmsWidget,
                     SLOT(openResourceContentByID(int)));

    t_ui ui;
    CMSWebToolbox* wtb = new CMSWebToolbox;
    ui.toolboxWidget = wtb;

    QObject::connect(wtb, &CMSWebToolbox::openPreview, this,
                     [=]() { openPluginWin(&m_CMSWeb, false); });
    QObject::connect(wtb, &CMSWebToolbox::openCurrentResource, this, [=]() {
        QUrl url = m_CMSWeb.url();
        if (url.hasQuery()) {
            QRegularExpression re("\\bres\\b=(\\d+)");
            QRegularExpressionMatch match = re.match(url.toString());
            if (match.hasMatch())
                emit openCurrentResource(match.captured(1).toInt());
        }
    });

    QObject::connect(mainWin, SIGNAL(logined(QString)), this,
                     SLOT(saveToken(QString)));
    QObject::connect(&m_CMSWeb, &CMSWebBrower::titleChanged, &m_CMSWeb,
                     &CMSWebBrower::setWindowTitle);
    QObject::connect(&m_CMSWeb, &CMSWebBrower::iconChanged, &m_CMSWeb,
                     &CMSWebBrower::setWindowIcon);
    QObject::connect(&m_CMSWeb, &CMSWebBrower::urlChanged, this,
                     [=](const QUrl& url) { qDebug() << url; });

    return ui;
}

void
CMSWebPlugin::setResource(const QVariantMap& resource, QWidget* widget) {
    /*QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);
    if (lineEdit)
        lineEdit->setText(resource["type"].toString() +
                          resource["id"].toString());*/
    QWebEngineView* previewWidget = qobject_cast<QWebEngineView*>(widget);
    if (resource["type"].toString() == "A" && previewWidget) {
        QString urlStr = QString("http://bgstudio.tk/resource.html?res=") +
                         resource["id"].toString();
        QObject::connect(
            this, &CMSWebPlugin::tokenChanged, this, [=](const QString&) {
                qDebug() << "--------------0" << urlStr + "&token=" + m_token;
                previewWidget->setUrl(QUrl(urlStr + "&token=" + m_token));
            });
        qDebug() << "--------------1" << urlStr + "&token=" + m_token;
        previewWidget->setUrl(QUrl(urlStr + "&token=" + m_token));
    }
}

void
CMSWebPlugin::saveToken(const QString& token) {
    m_token = token;
    m_CMSWeb.setUrl(QUrl("http://bgstudio.tk/?token=" + m_token));
    emit tokenChanged(token);
}

/*void
WebBrowerPlugin::openUrl(const QString& urlStr) {
    openPluginWin(m_webBrower, false);
    m_webBrower->setUrl(QUrl(urlStr));
}*/
