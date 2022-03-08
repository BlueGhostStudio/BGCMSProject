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
}

bool
FilterForm::check(const QVariantMap& node) const {
    QString type = node["type"].toString();
    QString contentType = node["contentType"].toString();
    int id = node["id"].toInt();

    bool contentTypeChecked;
    if (type == 'D')
        contentTypeChecked = true;
    else {
        if (contentType == "html")
            contentTypeChecked = ui_chbHtml->isChecked();
        else if (contentType == "txt")
            contentTypeChecked = ui_chbText->isChecked();
        else if (contentType == "css")
            contentTypeChecked = ui_chbCSS->isChecked();
        else if (contentType == "style")
            contentTypeChecked = ui_chbStyle->isChecked();
        else if (contentType == "md")
            contentTypeChecked = ui_chbMarkdown->isChecked();
        else if (contentType == "cmp")
            contentTypeChecked = ui_chbComponent->isChecked();
        else if (contentType == "pkg")
            contentTypeChecked = ui_chbPackage->isChecked();
        else if (contentType == "fra")
            contentTypeChecked = ui_chbFramework->isChecked();
        else
            contentTypeChecked = ui_chbOther->isChecked();
    }

    bool dotdotChecked;
    if (type == "D" && id <= 0)
        dotdotChecked = ui_chbDotDot->isChecked();
    else
        dotdotChecked = true;

    return contentTypeChecked && dotdotChecked;
    /*if (node == "html")
        return ui_chbHtml->isChecked();
    else if (node == "txt")
        return ui_chbText->isChecked();
    else if (node == "css")
        return ui_chbCSS->isChecked();
    else if (node == "style")
        return ui_chbStyle->isChecked();
    else if (node == "md")
        return ui_chbMarkdown->isChecked();
    else if (node == "cmp")
        return ui_chbComponent->isChecked();
    else if (node == "pkg")
        return ui_chbPackage->isChecked();
    else if (node == "fra")
        return ui_chbFramework->isChecked();
    else
        return ui_chbOther->isChecked();*/
}
