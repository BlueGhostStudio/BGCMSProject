/*****************************************************************************
 * newnodedlg.cpp
 *
 * Created: 2022/2/10 2022 by blueghost
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
#include "newnodedlg.h"

newNodeDlg::newNodeDlg(CallGraph *pcg, const QString &contentType,
                       const QString &accept, const QString &reject,
                       QWidget *parent)
    : CMSDialog(pcg, accept, reject, parent), m_contentType(contentType) {
    setupUi(this);
    ui_labContentType->setText(tr("Create new node") + " - " + contentType);
}

QVariant
newNodeDlg::acceptData() {
    return QVariantMap({ { "name", ui_leName->text() },
                         { "title", ui_leTitle->text() },
                         { "type", "F" },
                         { "contentType", m_contentType } });
}
