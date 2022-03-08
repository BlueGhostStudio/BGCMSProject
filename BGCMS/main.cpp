#include <bgmrpcclient.h>

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "mainwindow.h"

// NS_BGMRPCClient::BGMRPCClient BGCMSClient;

int
main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QApplication::setOrganizationName("BGStudio");
    QApplication::setOrganizationDomain("bgstudio.org");
    QApplication::setApplicationName("BGCMS");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BGCMS_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
