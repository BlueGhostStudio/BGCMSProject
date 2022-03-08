/*****************************************************************************
 * newnodedialog.cpp
 *
 * Created: 2022/1/27 2022 by blueghost
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
#include "newnodedialog.h"

NewNodeDialog::NewNodeDialog(CallGraph* pcg, const QString& accept,
                             const QString& reject, QWidget* parent)
    : CMSDialog(pcg, accept, reject, parent) {
    setupUi(this);
    ui_leResContent->installEventFilter(this);
}

QVariant
NewNodeDialog::acceptData() {
    QString type = ui_bgType->checkedButton()->property("value").toString();
    QVariantMap data = QVariantMap({ { "name", ui_leName->text() },
                                     { "title", ui_leTitle->text() },
                                     { "type", type } });
    if (type == "R") {
        QString content = ui_leResContent->text();
        if (content.contains(QRegularExpression("^https?://")))
            content = "link:" + content;
        QString contentType = content.section('.', -1);
        data["content"] = content;
        if (contentType.length() <= 5) data["contentType"] = contentType;
    }

    return data;
}
