/*****************************************************************************
 * infoform.cpp
 *
 * Created: 2022/3/7 2022 by blueghost
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
#include "infoform.h"

InfoForm::InfoForm(QWidget* parent) : QWidget(parent) {
    setupUi(this);

    QObject::connect(ui_pbSummaryReset, &QPushButton::clicked, this, [=]() {
        ui_pteSummary->document()->setPlainText(m_origSummary);
        ui_pteSummary->document()->setModified(false);
    });
    QObject::connect(ui_pbExtDataReset, &QPushButton::clicked, this, [=]() {
        ui_pteExtData->document()->setPlainText(m_origExtData);
        ui_pteExtData->document()->setModified(false);
    });

    QObject::connect(ui_pbSummaryUpdate, &QPushButton::clicked, this, [=]() {
        emit updateSummary(ui_pteSummary->toPlainText());
    });
    QObject::connect(ui_pbExtDataUpdate, &QPushButton::clicked, this, [=]() {
        emit updateExtData(ui_pteExtData->toPlainText());
    });
}

void
InfoForm::loadInfo(const QVariantMap& node) {
    if (!node.isEmpty()) {
        ui_twBaseInfo->item(0, 0)->setText(node["name"].toString());
        ui_twBaseInfo->item(1, 0)->setText(node["title"].toString());
        ui_twBaseInfo->item(2, 0)->setText(node["contentType"].toString());
        ui_twBaseInfo->item(3, 0)->setCheckState(
            node["hide"].toBool() ? Qt::Checked : Qt::Unchecked);
        ui_twBaseInfo->item(4, 0)->setCheckState(
            node["private"].toBool() ? Qt::Checked : Qt::Unchecked);
        ui_twBaseInfo->item(5, 0)->setText(node["own"].toString());
        ui_twBaseInfo->item(6, 0)->setText(node["date"].toString());
        ui_twBaseInfo->item(7, 0)->setText(node["mdate"].toString());
        ui_twBaseInfo->item(8, 0)->setText(node["seq"].toString());

        m_origSummary = node["summary"].toString();
        ui_pteSummary->document()->setPlainText(m_origSummary);
        ui_pteSummary->document()->setModified(false);
        m_origExtData = node["extData"].toString();
        ui_pteExtData->document()->setPlainText(m_origExtData);
        ui_pteExtData->document()->setModified(false);
    }
}
