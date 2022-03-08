/*****************************************************************************
 * filterform.cpp
 *
 * Created: 2022/2/21 2022 by blueghost
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
#include "filterform.h"

FilterForm::FilterForm(QWidget* parent) : QWidget(parent) {
    setupUi(this);
    QObject::connect(ui_bgCheckBoxs,
                     SIGNAL(buttonToggled(QAbstractButton*, bool)), this,
                     SIGNAL(updateFilter()));
    QObject::connect(ui_lwContentTypes, SIGNAL(itemChanged(QListWidgetItem*)),
                     this, SIGNAL(updateFilter()));
}

bool
FilterForm::check(const QVariantMap& node) const {
    QString type = node["type"].toString();
    QString contentType = node["contentType"].toString();
    int id = node["id"].toInt();
    int hide = node["hide"].toInt();
    int _private = node["private"].toInt();

    bool contentTypeChecked = false;
    if (type == 'D' || contentType.isEmpty())
        contentTypeChecked = true;
    else {
        if (ui_chbAllContentTypes->isChecked())
            contentTypeChecked = true;
        else
            for (int i = 0; i < ui_lwContentTypes->count(); i++) {
                QListWidgetItem* item = ui_lwContentTypes->item(i);
                if (item->text() == contentType) {
                    contentTypeChecked = (item->checkState() == Qt::Checked);
                    break;
                }
            }
    }

    bool dotdotChecked =
        type == "D" && id <= 0 ? ui_chbDotDot->isChecked() : true;

    bool hideChecked = hide ? ui_chbHide->isChecked() : true;
    bool privateChecked = _private ? ui_chbPrivate->isChecked() : true;

    return contentTypeChecked && dotdotChecked && hideChecked && privateChecked;
}

void
FilterForm::updateContentTypeList() {
    ui_lwContentTypes->clear();
    QSettings settings;
    settings.beginGroup("contentType");
    QVariantList types;
    foreach (const QString& key, settings.childKeys()) {
        types.append(settings.value(key).toList());
    }
    settings.endGroup();

    foreach (const QVariant& type, types) {
        QListWidgetItem* item = new QListWidgetItem(type.toString());
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(Qt::Checked);
        ui_lwContentTypes->addItem(item);
    }
}

void
FilterForm::checkAll() {
    for (int i = 0; i < ui_lwContentTypes->count(); i++) {
        QListWidgetItem* item = ui_lwContentTypes->item(i);
        item->setCheckState(Qt::Checked);
    }
}

void
FilterForm::uncheckAll() {
    for (int i = 0; i < ui_lwContentTypes->count(); i++) {
        QListWidgetItem* item = ui_lwContentTypes->item(i);
        item->setCheckState(Qt::Unchecked);
    }
}
