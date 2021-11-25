#include <QApplication>
#include <QLocale>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QTranslator>
#include <QtDebug>

#include "bgcmsmainwin.h"
#include "global.h"

NS_BGMRPCClient::BGMRPCClient bgmrpcClient;

int
main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    QApplication a(argc, argv);

    qDebug() << QStyleFactory::keys();

    QApplication::setOrganizationName("BGStudio");
    QApplication::setOrganizationDomain("bgstudio.org");
    QApplication::setApplicationName("BGCMS");

    qDebug() << QStandardPaths::standardLocations(
        QStandardPaths::AppConfigLocation);
    qDebug() << QStandardPaths::standardLocations(
        QStandardPaths::AppLocalDataLocation);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "BGCMS_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    BGCMSMainWin w;
    w.show();

    return a.exec();
}
