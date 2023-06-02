/*****************************************************************************
 * %{Cpp:License:FileName}
 *
 * Created: 2022/4/20 2022 by blueghost
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
#include "stylesheet.h"

#include <QApplication>
#include <QStandardPaths>

StyleSheet::StyleSheet(QObject* parent) : QObject(parent) {}

pluginUI
StyleSheet::initial(CMSApi* api, CMSBrowserBase* browser, const pluginUI& ui) {
    CMSPluginInterface::initial(api, browser);

    QObject::connect(&m_styleSheetWatch, &QFileSystemWatcher::fileChanged, this,
                     &StyleSheet::loadStyleSheet);

    QString qssFileName =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
        "/BGCMS.qss";
    m_styleSheetFile.setFileName(qssFileName);
    loadStyleSheet();

    return pluginUI();
}

void
StyleSheet::loadStyleSheet() {
    if (m_styleSheetFile.open(QIODevice::ReadOnly)) {
        QByteArray styleSheet = m_styleSheetFile.readAll();
        qApp->setStyleSheet(styleSheet);
        m_styleSheetFile.close();

        qDebug() << "----filename-----" << m_styleSheetFile.fileName();
        m_styleSheetWatch.addPath(m_styleSheetFile.fileName());
    }
}
