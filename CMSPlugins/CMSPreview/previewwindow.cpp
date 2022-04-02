/*****************************************************************************
 * previewwindow.cpp
 *
 * Created: 2022/3/24 2022 by blueghost
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
#include "previewwindow.h"

#include <QSettings>

PreviewWindow::PreviewWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi(this);

    ui_wevPreview->page()->setDevToolsPage(ui_wevDevTools->page());
    // ui_wevDevTools->hide();
    ui_dwDevTools->close();

    QObject::connect(ui_wevPreview, &QWebEngineView::loadStarted, this,
                     [=]() { loaded = false; });
    QObject::connect(ui_wevPreview, &QWebEngineView::loadFinished, this,
                     [=](bool ok) { loaded = ok; });

    QAction* action_inspect =
        ui_wevPreview->pageAction(QWebEnginePage::InspectElement);
    QObject::connect(action_inspect, &QAction::triggered, this,
                     [=]() { ui_dwDevTools->show(); });
    QObject::connect(ui_wevDevTools->page(),
                     &QWebEnginePage::windowCloseRequested, this,
                     [=]() { ui_dwDevTools->close(); });

    QSettings settings;

    restoreState(settings.value("preview/winState").toByteArray());
    restoreGeometry(settings.value("preview/geometry").toByteArray());
}

void
PreviewWindow::preview(const QString& path) {
    setWindowTitle(tr("Web Preview - ") + path);
    QUrl url = QSettings().value("preview/url", "http://127.0.0.1").toUrl();

    qDebug() << "-- preview --" << loaded << ui_wevPreview->url().host() << url
             << url.host();

    if (ui_wevPreview->url().host() != url.host() || !loaded) {
        url.setQuery("p=" + path);
        ui_wevPreview->setUrl(url);
    } else
        ui_wevPreview->page()->runJavaScript(
            "CMS.open(\"" + path + "\")",
            [=](const QVariant& ret) { qDebug() << ret; });

    raise();
}

void
PreviewWindow::closeEvent(QCloseEvent*) {
    QSettings settings;
    settings.setValue("preview/winState", saveState());
    settings.setValue("preview/geometry", saveGeometry());
}
